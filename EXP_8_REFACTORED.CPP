/* ============================================================
 * Experiment 8 : Cubic Bézier Curve
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   A cubic Bézier curve is defined by four control points
 *   P0, P1, P2, P3 and a parameter t ∈ [0, 1]:
 *
 *     B(t) = (1−t)³·P0  +  3(1−t)²t·P1
 *           + 3(1−t)t²·P2  +  t³·P3
 *
 *   P0 and P3 are the start and end points (the curve passes
 *   through them).  P1 and P2 are control handles that shape
 *   the curve's tangent directions but are generally NOT on
 *   the curve itself.
 *
 *   The control polygon P0–P1–P2–P3 is drawn as a visual aid.
 *
 *   Parameterisation note:
 *     The loop uses an integer step counter i = 0..BEZIER_STEPS
 *     and computes t = (float)i / BEZIER_STEPS.  This guarantees
 *     t == 0.0 at i=0 and t == 1.0 at i=BEZIER_STEPS exactly,
 *     avoiding accumulated floating-point error that can cause a
 *     floating-point loop (t += 0.001f) to miss the endpoint P3.
 *
 *   Colour key:  CYAN    = Bézier curve
 *                GREEN   = P0 (start) and P3 (end)
 *                YELLOW  = P1, P2 (control handles)
 *                DARKGRAY = Control polygon
 * ============================================================ */

#include <stdio.h>
#include <graphics.h>
#include <conio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define BGI_PATH      "C:\\TURBOC3\\BGI"

/* Number of line segments used to approximate the curve.
 * Higher = smoother curve; 1000 is sufficient for BGI resolution. */
#define BEZIER_STEPS  1000

/* ── Function Prototypes ────────────────────────────────────── */
void getControlPoints(int *x0, int *y0, int *x1, int *y1,
                      int *x2, int *y2, int *x3, int *y3);
void initCanvas      (void);
void markPoint       (int x, int y, int color);
void drawBezier      (int x0, int y0, int x1, int y1,
                      int x2, int y2, int x3, int y3);
void drawControlPolygon(int x0, int y0, int x1, int y1,
                        int x2, int y2, int x3, int y3);
void drawLegend      (void);

/* ============================================================
 * getControlPoints
 *   Reads the four Bézier control points from the user before
 *   initgraph() is called (Turbo C I/O restriction).
 * ============================================================ */
void getControlPoints(int *x0, int *y0, int *x1, int *y1,
                      int *x2, int *y2, int *x3, int *y3)
{
    printf("=== Bezier Curve (Cubic) ===\n\n");
    printf("Enter 4 control points (x y) each:\n\n");

    printf("  P0 - Start point     (x y): ");
    scanf("%d %d", x0, y0);

    printf("  P1 - Control handle  (x y): ");
    scanf("%d %d", x1, y1);

    printf("  P2 - Control handle  (x y): ");
    scanf("%d %d", x2, y2);

    printf("  P3 - End point       (x y): ");
    scanf("%d %d", x3, y3);
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
 * markPoint
 *   Draws a small circle marker at a control point to make
 *   its screen position clearly visible.
 * ============================================================ */
void markPoint(int x, int y, int color)
{
    setcolor(color);
    circle(x, y, 3);
}

/* ============================================================
 * drawBezier
 *   Rasterises the cubic Bézier curve by evaluating B(t) at
 *   BEZIER_STEPS+1 uniformly spaced values of t ∈ [0, 1] and
 *   plotting each resulting pixel.
 *
 *   Pre-computed per iteration:
 *     u  = 1 − t,  u² = u·u,  u³ = u²·u
 *     t² = t·t,    t³ = t²·t
 *   These avoid repeated multiplication and make the formula
 *   match the textbook form directly.
 *
 *   Bounds check: pixels outside the screen are skipped to
 *   prevent undefined behaviour from out-of-range putpixel().
 * ============================================================ */
void drawBezier(int x0, int y0, int x1, int y1,
                int x2, int y2, int x3, int y3)
{
    int i, px, py;
    float t, u, u2, u3, t2, t3;

    setcolor(CYAN);

    for (i = 0; i <= BEZIER_STEPS; i++)
    {
        /* Integer-driven t guarantees t == 0.0 and t == 1.0 exactly,
         * so both endpoints P0 and P3 are always plotted.           */
        t  = (float)i / (float)BEZIER_STEPS;
        u  = 1.0f - t;
        u2 = u  * u;
        u3 = u2 * u;
        t2 = t  * t;
        t3 = t2 * t;

        /* Cubic Bézier formula */
        px = (int)(u3 * x0  +  3.0f * u2 * t  * x1
                              +  3.0f * u  * t2 * x2  +  t3 * x3);
        py = (int)(u3 * y0  +  3.0f * u2 * t  * y1
                              +  3.0f * u  * t2 * y2  +  t3 * y3);

        /* Skip pixels outside the visible screen area */
        if (px < 0 || px >= getmaxx() || py < 0 || py >= getmaxy())
            continue;

        putpixel(px, py, CYAN);
    }
}

/* ============================================================
 * drawControlPolygon
 *   Draws the control polygon P0–P1–P2–P3 as a visual guide
 *   showing how the handles influence the curve's shape.
 * ============================================================ */
void drawControlPolygon(int x0, int y0, int x1, int y1,
                        int x2, int y2, int x3, int y3)
{
    setcolor(DARKGRAY);
    line(x0, y0, x1, y1);
    line(x1, y1, x2, y2);
    line(x2, y2, x3, y3);
}

/* ============================================================
 * drawLegend
 *   Renders a colour-matched key in the top-left corner.
 *   Each label is drawn in the colour it describes.
 * ============================================================ */
void drawLegend(void)
{
    int yPos;
    int LEGEND_Y_START = 10;
    int LEGEND_STEP    = 15;

    setcolor(CYAN);
    outtextxy(10, yPos = LEGEND_Y_START, "Cyan   = Bezier Curve");
    setcolor(GREEN);
    outtextxy(10, yPos += LEGEND_STEP,   "Green  = P0 (Start) and P3 (End)");
    setcolor(YELLOW);
    outtextxy(10, yPos += LEGEND_STEP,   "Yellow = P1, P2 (Control Handles)");
    setcolor(DARKGRAY);
    outtextxy(10, yPos + LEGEND_STEP,    "Gray   = Control Polygon");

    setcolor(WHITE);   /* restore default drawing colour */
}

/* ============================================================
 * main
 *   Orchestrates: input → canvas → curve → polygon →
 *                 markers → legend → wait → quit.
 * ============================================================ */
int main(void)
{
    int x0, y0, x1, y1, x2, y2, x3, y3;

    /* All scanf/printf MUST happen before initgraph() */
    getControlPoints(&x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);

    initCanvas();

    /* Draw curve first so the polygon and markers render on top */
    drawBezier(x0, y0, x1, y1, x2, y2, x3, y3);

    drawControlPolygon(x0, y0, x1, y1, x2, y2, x3, y3);

    /* Anchor points in GREEN; handles in YELLOW */
    markPoint(x0, y0, GREEN);
    markPoint(x1, y1, YELLOW);
    markPoint(x2, y2, YELLOW);
    markPoint(x3, y3, GREEN);

    drawLegend();

    getch();
    closegraph();

    return 0;
}
