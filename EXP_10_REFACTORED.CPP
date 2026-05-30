/* ============================================================
 * Experiment 10 : Animation — Car Moving on Road
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Scene Description (drawn back to front):
 *   • Light-blue sky with a yellow sun and rays
 *   • Three grey mountains with white snow caps
 *   • Green ground strip between mountains and road
 *   • Four houses (wall, roof, door, window) on the ground strip
 *   • Trees (trunk + canopy) between houses
 *   • Dark-grey road with white edge lines and animated dashes
 *   • Red car with wheels, windows, and lights moving right
 *   • Car wraps from the right edge back to the left
 *
 * Animation Technique — Double-Buffered Page Flipping:
 *   BGI on a VGA adapter provides two video pages (0 and 1).
 *   Each frame is drawn entirely on the INACTIVE page, then
 *   setvisualpage() flips to show it instantly, eliminating the
 *   blank-frame flicker produced by cleardevice() alone.
 *
 *   setactivepage(p) — future drawing goes to page p
 *   setvisualpage(p) — display switches to show page p
 *
 * Controls: Press any key to quit.
 * ============================================================ */

#include <graphics.h>
#include <conio.h>
#include <dos.h>

/* ── Layout constants ───────────────────────────────────────── */
#define BGI_PATH          "C:\\TURBOC3\\BGI"
#define SCREEN_W           640
#define SCREEN_H           480
#define ROAD_TOP           360
#define GROUND_TOP         320

/* ── Car constants ──────────────────────────────────────────── */
#define CAR_START_X       -120    /* left edge of car at entry  */
#define CAR_Y             (ROAD_TOP + 14) /* top of car body    */
#define CAR_STEP             3    /* pixels moved right/frame   */
#define CAR_WRAP_EXIT      660    /* x at which car wraps       */

/* ── Road dash constants ────────────────────────────────────── */
#define DASH_SPACING        50    /* pixels between dash starts */
#define DASH_LENGTH         28    /* pixels per dash            */
#define DASH_Y             420    /* y of centre-line dashes    */
#define DASH_SPEED           3    /* dash offset per frame      */

/* ── Animation timing ───────────────────────────────────────── */
#define FRAME_DELAY_MS      25    /* milliseconds between frames */

/* ── Function Prototypes ────────────────────────────────────── */
void initCanvas    (void);
void drawSky       (void);
void drawMountains (void);
void drawGround    (void);
void drawHouse     (int hx, int hy);
void drawTree      (int tx, int ty);
void drawRoad      (int dashOff);
void drawCar       (int cx, int cy);

/* ============================================================
 * initCanvas
 *   Initialises the BGI window.  DETECT on a VGA adapter
 *   selects VGAHI (640×480, 16 colours, 2 pages), which is
 *   required for page-flip double buffering.
 * ============================================================ */
void initCanvas(void)
{
    int graphDriver = DETECT, graphMode;
    initgraph(&graphDriver, &graphMode, BGI_PATH);
}

/* ============================================================
 * drawSky
 *   Fills the sky area with cyan, then draws the sun and its
 *   six rays with explicit setcolor calls before each element.
 * ============================================================ */
void drawSky(void)
{
    setcolor(CYAN);
    setfillstyle(SOLID_FILL, CYAN);
    bar(0, 0, SCREEN_W - 1, GROUND_TOP - 1);

    setcolor(YELLOW);
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(570, 55, 30, 30);

    setcolor(YELLOW);
    line(570, 15, 570,  5);
    line(610, 30, 620, 22);
    line(620, 70, 630, 78);
    line(570, 90, 570, 100);
    line(530, 70, 520, 78);
    line(520, 30, 510, 22);
}

/* ============================================================
 * drawMountains
 *   Draws three grey mountain triangles with white snow caps.
 *   Each mountain and its snow cap explicitly sets its own
 *   colour, so the draw order can be changed safely.
 * ============================================================ */
void drawMountains(void)
{
    /* Mountain bodies */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);

    {
        int pts[] = {  0, GROUND_TOP, 110, 120, 220, GROUND_TOP };
        fillpoly(3, pts);
    }
    {
        int pts[] = {160, GROUND_TOP, 310,  80, 460, GROUND_TOP };
        fillpoly(3, pts);
    }
    {
        int pts[] = {400, GROUND_TOP, 540, 110, 680, GROUND_TOP };
        fillpoly(3, pts);
    }

    /* Snow caps — drawn on top of each peak */
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);

    {
        int pts[] = { 90, 143, 110, 120, 130, 143 };
        fillpoly(3, pts);
    }
    {
        int pts[] = {288, 103, 310,  80, 332, 103 };
        fillpoly(3, pts);
    }
    {
        int pts[] = {518, 133, 540, 110, 562, 133 };
        fillpoly(3, pts);
    }
}

/* ============================================================
 * drawGround
 *   Green strip between the mountain bases and the road top.
 * ============================================================ */
void drawGround(void)
{
    setcolor(GREEN);
    setfillstyle(SOLID_FILL, GREEN);
    bar(0, GROUND_TOP, SCREEN_W - 1, ROAD_TOP - 1);
}

/* ============================================================
 * drawHouse
 *   Draws a house at position (hx, hy) where hy is the bottom
 *   of the walls.  Each part sets its own colour explicitly.
 *   bar() borders are matched to fill colour to prevent the
 *   default foreground outline from bleeding onto neighbours.
 * ============================================================ */
void drawHouse(int hx, int hy)
{
    int w  = 55;
    int h  = 45;

    /* Walls */
    setcolor(LIGHTGRAY);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    bar(hx, hy - h, hx + w, hy);

    /* Roof */
    setcolor(BROWN);
    setfillstyle(SOLID_FILL, BROWN);
    {
        int pts[] = { hx - 4,    hy - h,
                      hx + w/2,  hy - h - 28,
                      hx + w + 4, hy - h };
        fillpoly(3, pts);
    }

    /* Door */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(hx + 20, hy - 24, hx + 35, hy);

    /* Window fill */
    setcolor(CYAN);
    setfillstyle(SOLID_FILL, CYAN);
    bar(hx + 5, hy - 36, hx + 16, hy - 26);

    /* Window panes */
    setcolor(DARKGRAY);
    line(hx + 10, hy - 36, hx + 10, hy - 26);
    line(hx + 5,  hy - 31, hx + 16, hy - 31);
}

/* ============================================================
 * drawTree
 *   Draws a tree at (tx, ty) where ty is the base of the trunk.
 *   Three overlapping ellipses form a fuller canopy shape.
 * ============================================================ */
void drawTree(int tx, int ty)
{
    /* Trunk */
    setcolor(BROWN);
    setfillstyle(SOLID_FILL, BROWN);
    bar(tx - 4, ty - 30, tx + 4, ty);

    /* Canopy */
    setcolor(GREEN);
    setfillstyle(SOLID_FILL, GREEN);
    fillellipse(tx,      ty - 42, 18, 18);
    fillellipse(tx - 8,  ty - 34, 14, 14);
    fillellipse(tx + 8,  ty - 34, 14, 14);
}

/* ============================================================
 * drawRoad
 *   Draws the road surface, edge lines, and animated centre
 *   dashes.  dashOff advances each frame to simulate motion.
 *
 *   Dash x-position:  x = i - (dashOff % DASH_SPACING)
 *   Out-of-range dashes are wrapped so the pattern is seamless.
 * ============================================================ */
void drawRoad(int dashOff)
{
    int i, x;   /* declared at top of block — C89 compliant */

    /* Road surface */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(0, ROAD_TOP, SCREEN_W - 1, SCREEN_H - 1);

    /* Edge lines */
    setcolor(WHITE);
    line(0, ROAD_TOP,     SCREEN_W - 1, ROAD_TOP);
    line(0, SCREEN_H - 2, SCREEN_W - 1, SCREEN_H - 2);

    /* Animated centre dashes */
    for (i = -DASH_SPACING; i < SCREEN_W + DASH_SPACING; i += DASH_SPACING)
    {
        x = i - (dashOff % DASH_SPACING);
        if (x < -DASH_SPACING)
            x += SCREEN_W + DASH_SPACING;
        setcolor(WHITE);
        line(x, DASH_Y, x + DASH_LENGTH, DASH_Y);
    }
}

/* ============================================================
 * drawCar
 *   Draws a side-view car with body, cabin, windows, wheels,
 *   lights, and bumpers.  cx = left edge, cy = top of body.
 *   Every bar() call matches setcolor to fill colour to prevent
 *   unwanted outline artefacts from the BGI default foreground.
 * ============================================================ */
void drawCar(int cx, int cy)
{
    /* Main body */
    setcolor(RED);
    setfillstyle(SOLID_FILL, RED);
    bar(cx, cy + 18, cx + 110, cy + 48);

    /* Cabin (roof section — narrower and taller) */
    setcolor(RED);
    setfillstyle(SOLID_FILL, RED);
    bar(cx + 18, cy, cx + 88, cy + 20);

    /* Front windshield */
    setcolor(LIGHTBLUE);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    bar(cx + 66, cy + 2, cx + 86, cy + 18);

    /* Rear window */
    setcolor(LIGHTBLUE);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    bar(cx + 20, cy + 2, cx + 44, cy + 18);

    /* Wheel arches — dark cut-outs in the body */
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(cx + 22, cy + 48, 17, 10);
    fillellipse(cx + 88, cy + 48, 17, 10);

    /* Wheels — outer black tyre */
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(cx + 22, cy + 52, 14, 14);
    fillellipse(cx + 88, cy + 52, 14, 14);

    /* Wheel hubs — grey centre cap */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    fillellipse(cx + 22, cy + 52, 6, 6);
    fillellipse(cx + 88, cy + 52, 6, 6);

    /* Headlight — white, front */
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(cx + 107, cy + 30, 5, 4);

    /* Tail light — lighter red, rear */
    setcolor(LIGHTRED);
    setfillstyle(SOLID_FILL, LIGHTRED);
    fillellipse(cx + 3, cy + 30, 5, 4);

    /* Bumpers */
    setcolor(LIGHTGRAY);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    bar(cx + 100, cy + 40, cx + 112, cy + 46);
    bar(cx -   2, cy + 40, cx +  10, cy + 46);
}

/* ============================================================
 * main
 *   Initialises graphics, then runs the animation loop until
 *   a key is pressed.
 *
 *   Double-buffer pattern (per frame):
 *     1. setactivepage(page)  — point drawing to back buffer
 *     2. cleardevice()        — erase previous frame on that page
 *     3. draw everything      — render complete scene
 *     4. setvisualpage(page)  — flip: show the finished frame
 *     5. toggle page          — swap buffers for next frame
 *
 *   All variable declarations appear BEFORE any executable
 *   statement, as required by C89 (Turbo C).
 * ============================================================ */
int main(void)
{
    /* All declarations MUST precede any statement in C89 */
    int carX    = CAR_START_X;
    int dashOff = 0;
    int page    = 0;   /* active drawing page (0 or 1) */

    initCanvas();

    while (!kbhit())
    {
        /* Draw to the back page, then flip — eliminates flicker */
        page = 1 - page;
        setactivepage(page);
        cleardevice();

        /* Scene rendered back to front */
        drawSky();
        drawMountains();
        drawGround();

        drawHouse( 20, ROAD_TOP - 2);
        drawHouse(150, ROAD_TOP - 2);
        drawHouse(310, ROAD_TOP - 2);
        drawHouse(470, ROAD_TOP - 2);

        drawTree(  8, ROAD_TOP - 2);
        drawTree(120, ROAD_TOP - 2);
        drawTree(240, ROAD_TOP - 2);
        drawTree(400, ROAD_TOP - 2);
        drawTree(445, ROAD_TOP - 2);
        drawTree(560, ROAD_TOP - 2);
        drawTree(620, ROAD_TOP - 2);

        /* Road drawn after ground so it covers the green strip edge */
        drawRoad(dashOff);

        drawCar(carX, CAR_Y);

        setvisualpage(page);   /* atomic display swap */

        /* Advance animation state — both values kept bounded */
        carX    += CAR_STEP;
        dashOff  = (dashOff + DASH_SPEED) % (DASH_SPACING * 2);

        if (carX > CAR_WRAP_EXIT)
            carX = CAR_START_X;

        delay(FRAME_DELAY_MS);
    }

    getch();
    closegraph();
    return 0;
}
