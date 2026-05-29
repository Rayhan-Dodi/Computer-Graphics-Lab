/* ============================================================
 * Experiment 1B : Bresenham's Line Drawing Algorithm
 * ============================================================
 * Course  : Computer Graphics Lab
*
 * Theory  :
 *   Bresenham's algorithm avoids floating-point arithmetic by
 *   maintaining an integer "decision parameter" (d) that tracks
 *   the cumulative error between the ideal line and the pixel
 *   grid.  At each step along the dominant axis, d determines
 *   whether the minor axis should advance or stay.
 *
 *   Generalised to all 8 octants by:
 *     (a) Working with absolute deltas (absDx, absDy).
 *     (b) Deriving unit step signs from the original direction.
 *     (c) Swapping the role of x and y for steep lines
 *         (|dy| > |dx|) so the dominant axis always steps by 1.
 *
 *   Decision parameter (dominant axis = X, gentle slope):
 *     Initial : d = 2*absDy - absDx
 *     d < 0   : minor axis stays  → d += 2*absDy
 *     d >= 0  : minor axis steps  → d += 2*(absDy - absDx)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>     /* abs()  */
#include <graphics.h>
#include <conio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define BGI_PATH   "C:\\TURBOC3\\BGI"
#define LINE_COLOR WHITE

/* ── Macro: integer sign  (-1, 0, or +1) ───────────────────── */
#define SIGN(val)  ( ((val) > 0) - ((val) < 0) )

/* ── Function Prototypes ────────────────────────────────────── */
void getLineInput(int *x1, int *y1, int *x2, int *y2);
void initCanvas(void);
void drawLineBresenham(int x1, int y1, int x2, int y2);

/* ============================================================
 * getLineInput
 *   Prompts the user for integer start/end pixel coordinates.
 * ============================================================ */
void getLineInput(int *x1, int *y1, int *x2, int *y2)
{
    printf("=== Bresenham's Line Drawing Algorithm ===\n\n");
    printf("Enter starting point (x1 y1) : ");
    scanf("%d %d", x1, y1);
    printf("Enter ending   point (x2 y2) : ");
    scanf("%d %d", x2, y2);
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
 * drawLineBresenham
 *   Generalised 8-octant Bresenham rasterisation.
 *
 *   Steps:
 *     1. Compute absolute deltas and axis step directions.
 *     2. For steep lines, swap x/y roles so the dominant axis
 *        always advances by exactly 1 per iteration.
 *     3. Initialise decision parameter: d = 2*absDy - absDx
 *        (using dominant/minor naming after any swap).
 *     4. Loop over the dominant axis, plotting each pixel and
 *        updating d to decide minor-axis movement.
 *
 *   Edge case: identical endpoints → plot single pixel, return.
 * ============================================================ */
void drawLineBresenham(int x1, int y1, int x2, int y2)
{
    /* Guard: identical endpoints → single pixel */
    if (x1 == x2 && y1 == y2)
    {
        putpixel(x1, y1, LINE_COLOR);
        return;
    }

    int absDx = abs(x2 - x1);
    int absDy = abs(y2 - y1);

    /* Unit step directions derived from the true line direction */
    int xStep = SIGN(x2 - x1);
    int yStep = SIGN(y2 - y1);

    /* For steep lines (|dy| > |dx|), swap roles so we always
     * step along the longer axis, preventing pixel gaps.       */
    int steep = (absDy > absDx);
    if (steep)
    {
        int tmp;
        tmp = absDx; absDx = absDy; absDy = tmp;   /* swap deltas */
        tmp = xStep; xStep = yStep; yStep = tmp;    /* swap steps  */
    }

    /* Bresenham decision parameter for the dominant axis */
    int decisionParam = 2 * absDy - absDx;

    int x = x1;
    int y = y1;
    int i;

    for (i = 0; i <= absDx; i++)
    {
        /* Plot: un-swap coordinates if the line was steep */
        if (steep)
            putpixel(y, x, LINE_COLOR);   /* x holds dominant (was y) */
        else
            putpixel(x, y, LINE_COLOR);

        /* Update decision parameter and advance minor axis if needed */
        if (decisionParam >= 0)
        {
            y += yStep;                          /* minor axis steps  */
            decisionParam += 2 * (absDy - absDx);
        }
        else
        {
            decisionParam += 2 * absDy;          /* minor axis stays  */
        }

        x += xStep;   /* dominant axis always advances by 1 */
    }
}

/* ============================================================
 * main
 *   Orchestrates: input → canvas init → draw → wait → quit.
 * ============================================================ */
int main(void)
{
    int x1, y1, x2, y2;

    getLineInput(&x1, &y1, &x2, &y2);

    initCanvas();

    drawLineBresenham(x1, y1, x2, y2);

    getch();
    closegraph();

    return 0;
}
