/* ============================================================
 * Experiment 4 : 2D Geometric Transformations
 *                Translation · Rotation · Scaling
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   Three fundamental 2D transformations are applied to a
 *   user-defined polygon.  All operate on each vertex (x, y)
 *   independently and produce a new vertex set.
 *
 *   Translation  (shift by vector (tx, ty)):
 *       x' = x + tx,   y' = y + ty
 *
 *   Rotation (counter-clockwise around the screen origin):
 *       x' = x·cos θ − y·sin θ
 *       y' = x·sin θ + y·cos θ
 *     where θ is the angle in radians.
 *     Note: all four shapes share the same origin (0,0) so the
 *     rotated shape pivots around the top-left screen corner.
 *
 *   Scaling (uniform or non-uniform, from the screen origin):
 *       x' = x · sx,   y' = y · sy
 *
 *   Colour key:  WHITE  = Original
 *                GREEN  = Translated
 *                RED    = Rotated
 *                YELLOW = Scaled
 * ============================================================ */

#include <stdio.h>
#include <math.h>       /* cos(), sin() — return double  */
#include <graphics.h>
#include <conio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define BGI_PATH     "C:\\TURBOC3\\BGI"
#define MAX_VERTICES  20
#define PI            3.14159265f

/* ── Function Prototypes ────────────────────────────────────── */
void getPolygonInput   (int x[], int y[], int *n);
void getTransformParams(int *tx, int *ty, float *angle, float *sx, float *sy);
void initCanvas        (void);
void drawShape         (int x[], int y[], int n, int color);
void drawLegend        (void);
void translate         (int x[], int y[], int n, int tx, int ty,
                        int xt[], int yt[]);
void rotate            (int x[], int y[], int n, float angleDeg,
                        int xr[], int yr[]);
void scaleShape        (int x[], int y[], int n, float sx, float sy,
                        int xs[], int ys[]);

/* ============================================================
 * getPolygonInput
 *   Reads n vertices into x[] and y[].
 *   Enforces: 3 ≤ n ≤ MAX_VERTICES (minimum triangle).
 * ============================================================ */
void getPolygonInput(int x[], int y[], int *n)
{
    int i;

    printf("=== 2D Transformations: Translation, Rotation, Scaling ===\n\n");

    do
    {
        printf("Enter number of vertices (3 to %d): ", MAX_VERTICES);
        scanf("%d", n);
        if (*n < 3 || *n > MAX_VERTICES)
            printf("  [!] Must be between 3 and %d. Try again.\n",
                   MAX_VERTICES);
    }
    while (*n < 3 || *n > MAX_VERTICES);

    for (i = 0; i < *n; i++)
    {
        printf("  Vertex %d (x y): ", i + 1);
        scanf("%d %d", &x[i], &y[i]);
    }
}

/* ============================================================
 * getTransformParams
 *   Reads translation, rotation, and scaling parameters.
 * ============================================================ */
void getTransformParams(int *tx, int *ty,
                        float *angle, float *sx, float *sy)
{
    printf("\nEnter translation factors (tx ty)  : ");
    scanf("%d %d", tx, ty);

    printf("Enter rotation angle in degrees     : ");
    scanf("%f", angle);

    printf("Enter scaling factors   (sx sy)     : ");
    scanf("%f %f", sx, sy);
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
 * drawShape
 *   Draws a closed polygon by connecting consecutive vertices
 *   and closing the loop back to the first vertex.
 * ============================================================ */
void drawShape(int x[], int y[], int n, int color)
{
    int i;
    setcolor(color);

    for (i = 0; i < n - 1; i++)
    {
        line(x[i], y[i], x[i + 1], y[i + 1]);
    }
    /* Close the polygon: last vertex back to first */
    line(x[n - 1], y[n - 1], x[0], y[0]);
}

/* ============================================================
 * drawLegend
 *   Renders the colour key in the top-left corner.
 *   LEGEND_Y_START and LEGEND_STEP control vertical spacing.
 * ============================================================ */
void drawLegend(void)
{
    int yPos;
    int LEGEND_Y_START = 10;
    int LEGEND_STEP    = 15;

    setcolor(WHITE);
    outtextxy(10, yPos = LEGEND_Y_START,              "White  = Original");
    setcolor(GREEN);
    outtextxy(10, yPos += LEGEND_STEP,                "Green  = Translated");
    setcolor(RED);
    outtextxy(10, yPos += LEGEND_STEP,                "Red    = Rotated");
    setcolor(YELLOW);
    outtextxy(10, yPos + LEGEND_STEP,                 "Yellow = Scaled");

    setcolor(WHITE);   /* restore default drawing colour */
}

/* ============================================================
 * translate
 *   Shifts each vertex by the vector (tx, ty).
 *       x' = x + tx,  y' = y + ty
 * ============================================================ */
void translate(int x[], int y[], int n, int tx, int ty,
               int xt[], int yt[])
{
    int i;
    for (i = 0; i < n; i++)
    {
        xt[i] = x[i] + tx;
        yt[i] = y[i] + ty;
    }
}

/* ============================================================
 * rotate
 *   Rotates each vertex counter-clockwise around the origin
 *   by angleDeg degrees using the standard 2D rotation matrix.
 *   cos()/sin() return double; results are cast to float before
 *   being truncated to int, making the narrowing explicit.
 * ============================================================ */
void rotate(int x[], int y[], int n, float angleDeg,
            int xr[], int yr[])
{
    int i;
    float rad  = angleDeg * PI / 180.0f;
    float cosA = (float)cos(rad);   /* explicit double→float cast */
    float sinA = (float)sin(rad);

    for (i = 0; i < n; i++)
    {
        xr[i] = (int)(x[i] * cosA - y[i] * sinA);
        yr[i] = (int)(x[i] * sinA + y[i] * cosA);
    }
}

/* ============================================================
 * scaleShape
 *   Scales each vertex from the origin by factors (sx, sy).
 *       x' = x · sx,  y' = y · sy
 *   Renamed from 'scale' to avoid a name clash with the
 *   standard library function scale() on some compilers.
 * ============================================================ */
void scaleShape(int x[], int y[], int n, float sx, float sy,
                int xs[], int ys[])
{
    int i;
    for (i = 0; i < n; i++)
    {
        xs[i] = (int)(x[i] * sx);
        ys[i] = (int)(y[i] * sy);
    }
}

/* ============================================================
 * main
 *   Orchestrates: input → canvas init → transforms → draw.
 * ============================================================ */
int main(void)
{
    int   x[MAX_VERTICES],  y[MAX_VERTICES];   /* original    */
    int  xt[MAX_VERTICES], yt[MAX_VERTICES];   /* translated  */
    int  xr[MAX_VERTICES], yr[MAX_VERTICES];   /* rotated     */
    int  xs[MAX_VERTICES], ys[MAX_VERTICES];   /* scaled      */

    int   n;
    int   tx, ty;
    float angle, sx, sy;

    getPolygonInput(&x[0], &y[0], &n);
    getTransformParams(&tx, &ty, &angle, &sx, &sy);

    initCanvas();

    /* Apply and draw each transformation */
    drawShape(x,  y,  n, WHITE);

    translate(x, y, n, tx, ty, xt, yt);
    drawShape(xt, yt, n, GREEN);

    rotate(x, y, n, angle, xr, yr);
    drawShape(xr, yr, n, RED);

    scaleShape(x, y, n, sx, sy, xs, ys);
    drawShape(xs, ys, n, YELLOW);

    drawLegend();

    getch();
    closegraph();

    return 0;
}
