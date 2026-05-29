/* ============================================================
 * Experiment 5 : Shearing Transformation
 *                X-axis Shear · Y-axis Shear
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Theory  :
 *   Shearing distorts a shape along one axis while the
 *   perpendicular axis remains unchanged.  Only the coordinate
 *   along the shear axis is modified; the other is a direct
 *   copy, so the shape is "slanted" without area change.
 *
 *   X-axis Shear (horizontal — y is fixed, x shifts by shx·y):
 *       x' = x + shx · y
 *       y' = y
 *
 *   Y-axis Shear (vertical — x is fixed, y shifts by shy·x):
 *       x' = x
 *       y' = y + shy · x
 *
 *   Colour key:  WHITE   = Original
 *                CYAN    = X-Sheared
 *                MAGENTA = Y-Sheared
 * ============================================================ */

#include <stdio.h>
#include <graphics.h>
#include <conio.h>

/* ── Constants ─────────────────────────────────────────────── */
#define BGI_PATH      "C:\\TURBOC3\\BGI"
#define MAX_VERTICES   20

/* ── Function Prototypes ────────────────────────────────────── */
void getPolygonInput (int x[], int y[], int *n);
void getShearParams  (float *shx, float *shy);
void initCanvas      (void);
void drawShape       (int x[], int y[], int n, int color);
void drawLegend      (void);
void shearX          (int x[], int y[], int n, float shx,
                      int xOut[], int yOut[]);
void shearY          (int x[], int y[], int n, float shy,
                      int xOut[], int yOut[]);

/* ============================================================
 * getPolygonInput
 *   Reads n vertices into x[] and y[].
 *   Enforces: 3 ≤ n ≤ MAX_VERTICES (minimum triangle).
 * ============================================================ */
void getPolygonInput(int x[], int y[], int *n)
{
    int i;

    printf("=== Shearing Transformation ===\n\n");

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
 * getShearParams
 *   Reads X-axis and Y-axis shear factors from the user.
 * ============================================================ */
void getShearParams(float *shx, float *shy)
{
    printf("\nEnter X-axis shear factor (shx): ");
    scanf("%f", shx);
    printf("Enter Y-axis shear factor (shy): ");
    scanf("%f", shy);
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
 *   Renders a colour-matched key in the top-left corner.
 *   Each label is drawn in the colour it describes.
 * ============================================================ */
void drawLegend(void)
{
    int yPos;
    int LEGEND_Y_START = 10;
    int LEGEND_STEP    = 15;

    setcolor(WHITE);
    outtextxy(10, yPos = LEGEND_Y_START, "White   = Original");
    setcolor(CYAN);
    outtextxy(10, yPos += LEGEND_STEP,   "Cyan    = X-Sheared");
    setcolor(MAGENTA);
    outtextxy(10, yPos + LEGEND_STEP,    "Magenta = Y-Sheared");

    setcolor(WHITE);   /* restore default drawing colour */
}

/* ============================================================
 * shearX
 *   X-axis shear: shifts x proportionally to y; y is unchanged.
 *       x' = x + shx · y,   y' = y
 * ============================================================ */
void shearX(int x[], int y[], int n, float shx,
            int xOut[], int yOut[])
{
    int i;
    for (i = 0; i < n; i++)
    {
        xOut[i] = (int)(x[i] + shx * y[i]);
        yOut[i] = y[i];
    }
}

/* ============================================================
 * shearY
 *   Y-axis shear: shifts y proportionally to x; x is unchanged.
 *       x' = x,   y' = y + shy · x
 * ============================================================ */
void shearY(int x[], int y[], int n, float shy,
            int xOut[], int yOut[])
{
    int i;
    for (i = 0; i < n; i++)
    {
        xOut[i] = x[i];
        yOut[i] = (int)(y[i] + shy * x[i]);
    }
}

/* ============================================================
 * main
 *   Orchestrates: input → canvas init → shear → draw.
 * ============================================================ */
int main(void)
{
    int   x[MAX_VERTICES],   y[MAX_VERTICES];   /* original    */
    int xsx[MAX_VERTICES], ysx[MAX_VERTICES];   /* X-sheared   */
    int xsy[MAX_VERTICES], ysy[MAX_VERTICES];   /* Y-sheared   */

    int   n;
    float shx, shy;

    getPolygonInput(&x[0], &y[0], &n);
    getShearParams(&shx, &shy);

    initCanvas();

    drawShape(x,   y,   n, WHITE);

    shearX(x, y, n, shx, xsx, ysx);
    drawShape(xsx, ysx, n, CYAN);

    shearY(x, y, n, shy, xsy, ysy);
    drawShape(xsy, ysy, n, MAGENTA);

    drawLegend();

    getch();
    closegraph();

    return 0;
}
