/* ============================================================
 * Experiment 3 : Midpoint Circle Drawing Algorithm
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   The Midpoint Circle algorithm exploits the 8-way symmetry
 *   of a circle: only one octant (x: 0 → r/√2, y decreasing)
 *   is computed; each computed point (x, y) is immediately
 *   reflected into all 8 octants by sign permutations.
 *
 *   Starting at (0, r), the integer decision parameter tracks
 *   whether the midpoint between the two candidate pixels lies
 *   inside or outside the circle  x² + y² = r²:
 *
 *     Initial : d = 1 − r
 *
 *   Per iteration, x is incremented first, then d is tested
 *   using the new (post-increment) value of x:
 *     d < 0  : y stays  → d += 2·x_new + 1
 *     d >= 0 : y steps  → d += 2·x_new + 1 − 2·y_new
 *
 *   Using x_new and y_new in the update is essential; both
 *   reflect the pixel position that was just chosen.
 * ============================================================ */

#include <stdio.h>
#include <graphics.h>
#include <conio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define BGI_PATH    "C:\\TURBOC3\\BGI"
#define CIRCLE_COLOR WHITE

/* ── Function Prototypes ────────────────────────────────────── */
void getCircleInput(int *xc, int *yc, int *r);
void initCanvas(void);
void plotSymmetricPoints(int xc, int yc, int x, int y);
void drawCircleMidpoint(int xc, int yc, int r);

/* ============================================================
 * getCircleInput
 *   Prompts for circle centre and radius; rejects r <= 0.
 * ============================================================ */
void getCircleInput(int *xc, int *yc, int *r)
{
    printf("=== Midpoint Circle Drawing Algorithm ===\n\n");
    printf("Enter centre (xc yc) : ");
    scanf("%d %d", xc, yc);

    do
    {
        printf("Enter radius (> 0)   : ");
        scanf("%d", r);
        if (*r <= 0)
            printf("  [!] Radius must be a positive integer. Try again.\n");
    }
    while (*r <= 0);
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
 * plotSymmetricPoints
 *   Given one computed octant point (x, y), reflects it into
 *   all 8 octants using the circle's 8-way symmetry.
 *   The four ±x/±y combinations cover the axis-aligned pairs;
 *   swapping x and y covers the diagonal pairs.
 * ============================================================ */
void plotSymmetricPoints(int xc, int yc, int x, int y)
{
    /* Axis-aligned octants (top, bottom, left, right arcs) */
    putpixel(xc + x, yc + y, CIRCLE_COLOR);
    putpixel(xc - x, yc + y, CIRCLE_COLOR);
    putpixel(xc + x, yc - y, CIRCLE_COLOR);
    putpixel(xc - x, yc - y, CIRCLE_COLOR);

    /* Diagonal octants (swap x and y to reflect across y = x) */
    putpixel(xc + y, yc + x, CIRCLE_COLOR);
    putpixel(xc - y, yc + x, CIRCLE_COLOR);
    putpixel(xc + y, yc - x, CIRCLE_COLOR);
    putpixel(xc - y, yc - x, CIRCLE_COLOR);
}

/* ============================================================
 * drawCircleMidpoint
 *   Core midpoint circle rasterisation for one octant.
 *   Iterates while x < y (i.e., until the 45° diagonal),
 *   plotting all 8 reflected pixels at each step.
 * ============================================================ */
void drawCircleMidpoint(int xc, int yc, int r)
{
    int x = 0;
    int y = r;

    /* Initial decision parameter: derived from evaluating the
     * circle equation at the midpoint of the first two
     * candidate pixels: (1, r) vs (1, r−1).               */
    int decisionParam = 1 - r;

    /* Plot the starting point (top of circle) and its reflections */
    plotSymmetricPoints(xc, yc, x, y);

    while (x < y)
    {
        x++;   /* dominant axis always advances by 1 */

        if (decisionParam < 0)
        {
            /* Midpoint is inside the circle: stay on current y.
             * Update uses the new x (post-increment).          */
            decisionParam += 2 * x + 1;
        }
        else
        {
            /* Midpoint is outside or on the circle: step y inward.
             * Update uses both new x and new y (post-decrement). */
            y--;
            decisionParam += 2 * x + 1 - 2 * y;
        }

        plotSymmetricPoints(xc, yc, x, y);
    }
}

/* ============================================================
 * main
 *   Orchestrates: input → canvas init → draw → wait → quit.
 * ============================================================ */
int main(void)
{
    int xc, yc, r;

    getCircleInput(&xc, &yc, &r);

    initCanvas();

    drawCircleMidpoint(xc, yc, r);

    getch();
    closegraph();

    return 0;
}
