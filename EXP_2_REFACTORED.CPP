/* ============================================================
 * Experiment 2 : Midpoint Line Drawing Algorithm
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   The Midpoint algorithm decides which of two candidate pixels
 *   to plot by evaluating the implicit line equation
 *       F(x, y) = dy·x  −  dx·y  +  dx·y1  −  dy·x1
 *   at the midpoint between them.
 *
 *   For the gentle-slope case (dominant axis = X):
 *     Candidates after pixel (xk, yk) : E=(xk+1, yk)
 *                                        NE=(xk+1, yk+1)
 *     Midpoint M = (xk+1,  yk + 0.5)
 *
 *   Naïve initial parameter:   d = dy − dx/2
 *   Problem: dx/2 introduces a fraction for odd dx, breaking
 *   the integer-only guarantee.
 *
 *   Solution — scale by 2 to eliminate the fraction:
 *     d_init = 2·absDy − absDx          (no fraction ever)
 *     d < 0  : E  chosen → d += 2·absDy
 *     d >= 0 : NE chosen → d += 2·(absDy − absDx)
 *
 *   Generalised to all 8 octants with:
 *     (a) Absolute deltas (absDx, absDy).
 *     (b) Sign-derived unit steps (xStep, yStep).
 *     (c) Axis-swap for steep lines (|dy| > |dx|).
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
void drawLineMidpoint(int x1, int y1, int x2, int y2);

/* ============================================================
 * getLineInput
 *   Prompts the user for integer start/end pixel coordinates.
 * ============================================================ */
void getLineInput(int *x1, int *y1, int *x2, int *y2)
{
    printf("=== Midpoint Line Drawing Algorithm ===\n\n");
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
 * drawLineMidpoint
 *   Generalised 8-octant Midpoint rasterisation.
 *
 *   Steps:
 *     1. Compute absolute deltas and axis step directions.
 *     2. For steep lines, swap x/y roles so the dominant axis
 *        always advances by exactly 1 per iteration.
 *     3. Initialise the ×2-scaled decision parameter:
 *           d = 2·absDy − absDx
 *        (scaling by 2 eliminates the dx/2 fraction).
 *     4. Loop over the dominant axis, plotting each pixel and
 *        updating d to choose minor-axis movement.
 *
 *   Edge case: identical endpoints → plot single pixel, return.
 * ============================================================ */
void drawLineMidpoint(int x1, int y1, int x2, int y2)
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

    /* ×2-scaled midpoint decision parameter (no integer division).
     * Equivalent to evaluating 2·F at the first candidate midpoint. */
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

        /* Midpoint test: choose E or NE (or S/SE for other octants) */
        if (decisionParam >= 0)
        {
            y += yStep;                              /* NE: minor steps  */
            decisionParam += 2 * (absDy - absDx);
        }
        else
        {
            decisionParam += 2 * absDy;              /* E : minor stays  */
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

    drawLineMidpoint(x1, y1, x2, y2);

    getch();
    closegraph();

    return 0;
}
