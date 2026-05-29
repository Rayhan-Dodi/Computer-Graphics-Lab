/* ============================================================
 * Experiment 7A : Boundary Fill Algorithm (8-Connected)
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   Boundary fill floods a region from a seed point, painting
 *   every pixel that is neither the boundary colour nor already
 *   the fill colour.  It stops when it hits the drawn border.
 *
 *   8-connectivity considers all 8 surrounding pixels:
 *       NW  N  NE
 *        W  *  E
 *       SW  S  SE
 *
 *   Stop condition for each candidate pixel (nx, ny):
 *       DO fill   if pixel ≠ boundaryColor AND pixel ≠ fillColor
 *       DON'T fill if pixel == boundaryColor (border reached)
 *                  or pixel == fillColor    (already painted)
 *
 *   Contrast with Flood Fill (EXP_6A/6B):
 *     Flood fill checks for a specific interior colour (oldColor).
 *     Boundary fill checks for the *absence* of the border colour,
 *     so it works with any multi-coloured interior.
 *
 *   Implementation — Iterative Stack (NOT recursive):
 *     A recursive boundary fill on a screen-sized region requires
 *     one call frame per pixel (~9,600 frames for a 100×100
 *     interior).  Turbo C's default call stack is only ~4 KB,
 *     so recursion causes an immediate stack overflow and crash.
 *
 *     The fix is an explicit LIFO stack (two global int arrays).
 *     Each pixel is painted BEFORE being pushed, so no pixel is
 *     ever pushed twice.  Stack depth is bounded by fill area.
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
 * comfortable margin.  Increase for larger shapes and use
 * Turbo C's LARGE memory model if needed.                   */
#define STACK_SIZE  12000

/* ── Global explicit stack (avoids Turbo C call-stack limits) ─ */
static int stackX[STACK_SIZE];
static int stackY[STACK_SIZE];
static int stackTop;   /* index of next free slot; 0 = empty  */

/* 8-connected neighbour offsets: 4 cardinal + 4 diagonal */
static int neighborDX[8] = {  1, -1,  0,  0,  1, -1,  1, -1 };
static int neighborDY[8] = {  0,  0,  1, -1,  1, -1, -1,  1 };

/* ── Function Prototypes ────────────────────────────────────── */
void getUserInput   (int *seedX, int *seedY,
                     int *fillColor, int *boundaryColor);
void initCanvas     (void);
void boundaryFill8  (int seedX, int seedY,
                     int fillColor, int boundaryColor);

/* ============================================================
 * getUserInput
 *   All input is collected BEFORE initgraph() because Turbo C
 *   cannot use printf/scanf once graphics mode is active.
 *
 *   Validates:
 *     • Seed is strictly inside the demo rectangle.
 *     • fillColor and boundaryColor are valid BGI indices (0–15).
 *     • fillColor ≠ boundaryColor (otherwise fill never stops).
 * ============================================================ */
void getUserInput(int *seedX, int *seedY,
                  int *fillColor, int *boundaryColor)
{
    printf("=== Boundary Fill Algorithm (8-Connected) ===\n\n");
    printf("A rectangle will be drawn from (%d,%d) to (%d,%d).\n\n",
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

    /* Boundary colour must be a valid BGI colour index */
    do
    {
        printf("Enter boundary colour (0=BLACK to 15=WHITE): ");
        scanf("%d", boundaryColor);
        if (*boundaryColor < 0 || *boundaryColor > 15)
            printf("  [!] Colour must be between 0 and 15.\n");
    }
    while (*boundaryColor < 0 || *boundaryColor > 15);

    /* Fill colour must differ from boundary colour */
    do
    {
        printf("Enter fill colour    (0=BLACK to 15=WHITE): ");
        scanf("%d", fillColor);
        if (*fillColor < 0 || *fillColor > 15)
            printf("  [!] Colour must be between 0 and 15.\n");
        else if (*fillColor == *boundaryColor)
            printf("  [!] Fill colour must differ from boundary colour.\n");
    }
    while (*fillColor < 0 || *fillColor > 15 ||
           *fillColor == *boundaryColor);
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
 * boundaryFill8
 *   Iterative 8-connected boundary fill using an explicit stack.
 *
 *   Algorithm:
 *     1. Guard: return if seed pixel is already boundaryColor
 *               or already fillColor (nothing to fill).
 *     2. Paint seed to fillColor immediately; push onto stack.
 *     3. While stack is not empty:
 *          a. Pop the top pixel (x, y).
 *          b. For each of its 8 neighbours (nx, ny):
 *               • Skip if out of screen bounds.
 *               • Read pixel colour.
 *               • If it is NEITHER boundaryColor NOR fillColor:
 *                   – Paint to fillColor immediately (prevents
 *                     the same pixel being pushed more than once).
 *                   – Push (nx, ny) onto stack.
 *
 *   The "paint before push" rule is the key correctness invariant:
 *   it guarantees each pixel is enqueued at most once, keeping
 *   peak stack usage proportional to the fill-area perimeter
 *   rather than its total area.
 * ============================================================ */
void boundaryFill8(int seedX, int seedY, int fillColor, int boundaryColor)
{
    int x, y, nx, ny, i, pixelColor;

    /* Guard: seed is already on the boundary or already filled */
    pixelColor = getpixel(seedX, seedY);
    if (pixelColor == boundaryColor || pixelColor == fillColor)
        return;

    /* Seed: paint immediately, then push */
    putpixel(seedX, seedY, fillColor);
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

        /* Process all 8 neighbours */
        for (i = 0; i < 8; i++)
        {
            nx = x + neighborDX[i];
            ny = y + neighborDY[i];

            /* Bounds check: prevent out-of-screen getpixel() */
            if (nx < 0 || nx >= SCREEN_W || ny < 0 || ny >= SCREEN_H)
                continue;

            pixelColor = getpixel(nx, ny);

            /* Fill only if not on the boundary and not already filled */
            if (pixelColor != boundaryColor && pixelColor != fillColor)
            {
                /* Paint before pushing to prevent duplicate entries */
                putpixel(nx, ny, fillColor);

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
 *                 boundary fill → wait → quit.
 * ============================================================ */
int main(void)
{
    int seedX, seedY, fillColor, boundaryColor;

    /* All scanf/printf MUST happen before initgraph() */
    getUserInput(&seedX, &seedY, &fillColor, &boundaryColor);

    initCanvas();

    /* Draw the boundary rectangle in the user-chosen colour */
    setcolor(boundaryColor);
    rectangle(RECT_X1, RECT_Y1, RECT_X2, RECT_Y2);

    boundaryFill8(seedX, seedY, fillColor, boundaryColor);

    getch();
    closegraph();

    return 0;
}
