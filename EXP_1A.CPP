/* ============================================================
 * Experiment 1A : DDA Line Drawing Algorithm
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   DDA draws a line by incrementally stepping one pixel at a
 *   time along the dominant axis (the axis with the larger span).
 *   The other axis advances by a fractional increment each step,
 *   derived from the ratio  dy/dx  or  dx/dy.  Rounding via
 *   +0.5 maps each floating-point position to the nearest pixel.
 * ============================================================ */

#include <stdio.h>
#include <math.h>       /* fabs()  */
#include <graphics.h>
#include <conio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define BGI_PATH   "C:\\TURBOC3\\BGI"
#define LINE_COLOR WHITE

/* ── Function Prototypes ────────────────────────────────────── */
void getLineInput(float *x1, float *y1, float *x2, float *y2);
void initCanvas(void);
void drawLineDDA(float x1, float y1, float x2, float y2);

/* ============================================================
 * getLineInput
 *   Prompts the user for the start and end coordinates of the
 *   line to be drawn.
 * ============================================================ */
void getLineInput(float *x1, float *y1, float *x2, float *y2)
{
    printf("=== DDA Line Drawing Algorithm ===\n\n");
    printf("Enter starting point (x1 y1) : ");
    scanf("%f %f", x1, y1);
    printf("Enter ending   point (x2 y2) : ");
    scanf("%f %f", x2, y2);
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
 * drawLineDDA
 *   Core DDA rasterisation:
 *     1. Compute raw deltas (signed).
 *     2. Choose the larger absolute delta as the step count.
 *     3. Derive per-step increments:  xInc = dx/steps,
 *                                     yInc = dy/steps.
 *     4. Walk from (x1,y1) to (x2,y2), plotting each pixel.
 *
 *   Edge case: identical endpoints → plot single pixel, return.
 * ============================================================ */
void drawLineDDA(float x1, float y1, float x2, float y2)
{
    float dx    = x2 - x1;
    float dy    = y2 - y1;

    /* Guard: if both endpoints are identical, plot one pixel */
    float steps = (fabs(dx) > fabs(dy)) ? fabs(dx) : fabs(dy);
    if (steps == 0)
    {
        putpixel((int)(x1 + 0.5f), (int)(y1 + 0.5f), LINE_COLOR);
        return;
    }

    /* Per-step increments along each axis */
    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = x1;
    float y = y1;

    /* Plot all pixels from start to end (inclusive) */
    int i;
    for (i = 0; i <= (int)steps; i++)
    {
        putpixel((int)(x + 0.5f), (int)(y + 0.5f), LINE_COLOR);
        x += xInc;
        y += yInc;
    }
}

/* ============================================================
 * main
 *   Orchestrates: input → graphics init → draw → wait → quit.
 * ============================================================ */
int main(void)
{
    float x1, y1, x2, y2;

    getLineInput(&x1, &y1, &x2, &y2);

    initCanvas();

    drawLineDDA(x1, y1, x2, y2);

    getch();
    closegraph();

    return 0;
}