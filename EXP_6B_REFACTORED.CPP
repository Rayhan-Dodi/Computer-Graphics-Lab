/* ============================================================
 * Experiment 6B : Flood Fill Algorithm (4-Connected)
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   Flood fill replaces all pixels of 'oldColor' that are
 *   4-connected to the seed point with 'newColor'.
 *
 *   4-connectivity considers only the 4 cardinal neighbours:
 *          N
 *       W  *  E
 *          S
 *   Diagonal neighbours are NOT included (contrast with 6B's
 *   8-connected variant).  This means 4-fill cannot "leak"
 *   through a diagonal gap of one pixel.
 *
 *   Implementation — Iterative Stack (NOT recursive):
 *     A recursive flood fill on a screen-sized region requires
 *     one call frame per pixel (~9,600 frames for a 100×100
 *     interior).  Turbo C's default call stack is only ~4 KB,
 *     so recursion causes an immediate stack overflow and crash.
 *
 *     The fix is an explicit LIFO stack (two global int arrays).
 *     Each pixel is painted to 'newColor' BEFORE being pushed,
 *     so no pixel is ever pushed twice.  Stack depth is bounded
 *     by the fill area, not the call depth.
 *
 * ============================================================ */

#include <stdio.h>
#include <graphics.h>
#include <conio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define BGI_PATH    "C:\\TURBOC3\\BGI"

/* VGA screen dimensions used for bounds-checking */
#define SCREEN_W    640
#define SCREEN_H    480

/* Demo rectangle coordinates */
#define RECT_X1     100
#define RECT_Y1     100
#define RECT_X2     200
#define RECT_Y2     200

/* Stack capacity: must cover the largest possible fill area.
 * The demo interior is 98×98 = 9,604 pixels; 12,000 gives a
 * comfortable margin.  For larger shapes, increase this value
 * and compile with Turbo C's LARGE memory model.            */
#define STACK_SIZE  12000

/* ── Global explicit stack (avoids Turbo C call-stack limits) ─ */
static int stackX[STACK_SIZE];
static int stackY[STACK_SIZE];
static int stackTop;   /* index of next free slot; 0 = empty  */

/* 4-connected neighbour offsets: cardinal directions only */
static int neighborDX[4] = {  1, -1,  0,  0 };
static int neighborDY[4] = {  0,  0,  1, -1 };

/* ── Function Prototypes ────────────────────────────────────── */
void getUserInput(int *seedX, int *seedY, int *fillColor);
void initCanvas  (void);
void floodFill4  (int seedX, int seedY, int oldColor, int newColor);

/* ============================================================
 * getUserInput
 *   All input is collected BEFORE initgraph() because Turbo C
 *   cannot use printf/scanf once graphics mode is active.
 *
 *   Validates:
 *     • Seed point is strictly inside the demo rectangle.
 *     • Fill colour is a valid BGI colour index (0–15).
 * ============================================================ */
void getUserInput(int *seedX, int *seedY, int *fillColor)
{
    printf("=== Flood Fill Algorithm (4-Connected) ===\n\n");
    printf("A WHITE rectangle will be drawn from (%d,%d) to (%d,%d).\n\n",
           RECT_X1, RECT_Y1, RECT_X2, RECT_Y2);

    /* Seed must be strictly inside the rectangle boundary */
    do
    {
        printf("Enter seed point inside the shape (x y): ");
        scanf("%d %d", seedX, seedY);
        if (*seedX <= RECT_X1 || *seedX >= RECT_X2 ||
            *seedY <= RECT_Y1 || *seedY >= RECT_Y2)
            printf("  [!] Seed must be strictly inside (%d,%d)–(%d,%d).\n",
                   RECT_X1 + 1, RECT_Y1 + 1, RECT_X2 - 1, RECT_Y2 - 1);
    }
    while (*seedX <= RECT_X1 || *seedX >= RECT_X2 ||
           *seedY <= RECT_Y1 || *seedY >= RECT_Y2);

    /* Fill colour must be a valid BGI colour index */
    do
    {
        printf("Enter fill colour (0=BLACK to 15=WHITE): ");
        scanf("%d", fillColor);
        if (*fillColor < 0 || *fillColor > 15)
            printf("  [!] Colour must be between 0 and 15.\n");
    }
    while (*fillColor < 0 || *fillColor > 15);
}

/* ============================================================
 * initCanvas
 *   Initialises the BGI graphics window using auto-detection.
 * ============================================================ */
void initCanvas(void)
{
    int graphDriver = DETECT, graphMode;
    initgraph(&graphDriver, &graphMode, BGI_PATH);
}

/* ============================================================
 * floodFill4
 *   Iterative 4-connected flood fill using an explicit stack.
 *
 *   Algorithm:
 *     1. Guard: return if oldColor == newColor (nothing to do),
 *               or seed pixel is not oldColor.
 *     2. Paint seed to newColor immediately; push onto stack.
 *     3. While stack is not empty:
 *          a. Pop the top pixel (x, y).
 *          b. For each of its 4 cardinal neighbours (nx, ny):
 *               • Skip if out of screen bounds.
 *               • If getpixel(nx, ny) == oldColor:
 *                   – Paint to newColor immediately (prevents
 *                     the same pixel being pushed more than once).
 *                   – Push (nx, ny) onto stack.
 *
 *   Painting before pushing ensures each pixel is pushed at
 *   most once, bounding memory to the fill-area size.
 * ============================================================ */
void floodFill4(int seedX, int seedY, int oldColor, int newColor)
{
    int x, y, nx, ny, i;

    /* Guard: identical colours → nothing to change */
    if (oldColor == newColor)
        return;

    /* Guard: seed pixel is not the target colour */
    if (getpixel(seedX, seedY) != oldColor)
        return;

    /* Seed: paint immediately, then push */
    putpixel(seedX, seedY, newColor);
    stackTop         = 0;
    stackX[stackTop] = seedX;
    stackY[stackTop] = seedY;
    stackTop++;

    while (stackTop > 0)
    {
        /* Pop */
        stackTop--;
        x = stackX[stackTop];
        y = stackY[stackTop];

        /* Process all 4 cardinal neighbours */
        for (i = 0; i < 4; i++)
        {
            nx = x + neighborDX[i];
            ny = y + neighborDY[i];

            /* Bounds check: prevent out-of-screen getpixel() */
            if (nx < 0 || nx >= SCREEN_W || ny < 0 || ny >= SCREEN_H)
                continue;

            if (getpixel(nx, ny) == oldColor)
            {
                /* Paint before pushing to prevent duplicate entries */
                putpixel(nx, ny, newColor);

                if (stackTop < STACK_SIZE)
                {
                    stackX[stackTop] = nx;
                    stackY[stackTop] = ny;
                    stackTop++;
                }
                /* If STACK_SIZE is exceeded, this pixel's neighbours
                 * are skipped.  Increase STACK_SIZE for larger fills. */
            }
        }
    }
}

/* ============================================================
 * main
 *   Orchestrates: input → canvas → draw boundary →
 *                 read oldColor → flood fill → wait → quit.
 * ============================================================ */
int main(void)
{
    int seedX, seedY, fillColor;
    int oldColor;

    /* All scanf/printf MUST happen before initgraph() */
    getUserInput(&seedX, &seedY, &fillColor);

    initCanvas();

    /* Draw the boundary rectangle in WHITE */
    setcolor(WHITE);
    rectangle(RECT_X1, RECT_Y1, RECT_X2, RECT_Y2);

    /* Read oldColor AFTER initgraph and AFTER drawing the shape,
     * so getpixel() reflects the actual rendered background.    */
    oldColor = getpixel(seedX, seedY);

    floodFill4(seedX, seedY, oldColor, fillColor);

    getch();
    closegraph();

    return 0;
}
