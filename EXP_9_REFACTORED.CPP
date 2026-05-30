/* ============================================================
 * Experiment 9 : Animation — Man Walking on a Rainy Day
 * ============================================================
 * Course  : Computer Graphics Lab
 *
 * Scene Description:
 *   • Dark sky with an overcast sun
 *   • Three dark-grey cloud banks (overlapping ellipses)
 *   • Animated diagonal rain streaks falling right-to-left
 *   • A grey road with a centre divider line
 *   • A stick-figure man holding a red umbrella, walking
 *     right-to-left with an alternating 2-phase leg cycle
 *   • Man wraps around from the left edge to the right edge
 *
 * Animation Technique — Double-Buffered Page Flipping:
 *   BGI on a VGA adapter provides two video pages (0 and 1).
 *   Each frame is drawn entirely on the INACTIVE page, then
 *   setvisualpage() flips to show it instantly.  The display
 *   therefore always shows a complete frame — the blank
 *   intermediate state that causes flicker with cleardevice()
 *   alone is never visible.
 *
 *   setactivepage(p) — future drawing goes to page p
 *   setvisualpage(p) — display switches to show page p
 *
 * Controls:  Press any key to quit.
 * ============================================================ */

#include <graphics.h>
#include <conio.h>
#include <dos.h>

/* ── Layout constants ───────────────────────────────────────── */
#define BGI_PATH        "C:\\TURBOC3\\BGI"
#define SCREEN_W         640
#define SCREEN_H         480
#define GROUND_Y         400    /* top edge of road surface     */

/* ── Man constants ──────────────────────────────────────────── */
#define MAN_START_X      620    /* initial x of man's shoulder  */
#define MAN_Y            342    /* fixed shoulder y (feet on road) */
#define MAN_STEP           2    /* pixels moved left per frame  */
#define MAN_WRAP_EXIT    -60    /* x at which man wraps         */
#define MAN_WRAP_ENTRY   660    /* x at which man re-enters     */

/* ── Rain constants ─────────────────────────────────────────── */
#define RAIN_ROW_GAP      18    /* vertical spacing between rows */
#define RAIN_COL_GAP      25    /* horizontal spacing between drops */
#define RAIN_SPEED         3    /* rain offset increment per frame */

/* ── Animation timing ───────────────────────────────────────── */
#define FRAME_DELAY_MS    35    /* milliseconds between frames   */
#define LEG_TOGGLE_FRAMES  8    /* frames per leg-phase toggle   */

/* ── Function Prototypes ────────────────────────────────────── */
void initCanvas        (void);
void drawBackground    (void);
void drawRain          (int offset);
void drawMan           (int mx, int my, int legPhase);

/* ============================================================
 * initCanvas
 *   Initialises the BGI graphics window.  DETECT on a VGA
 *   adapter selects VGAHI (640×480, 16 colours, 2 pages),
 *   which is required for page-flip double buffering.
 * ============================================================ */
void initCanvas(void)
{
    int graphDriver = DETECT, graphMode;
    initgraph(&graphDriver, &graphMode, BGI_PATH);
}

/* ============================================================
 * drawBackground
 *   Draws the static scene elements: sky, sun, clouds, road.
 *   Each cloud group explicitly re-sets its colour so the
 *   draw order can be changed without breaking any group.
 * ============================================================ */
void drawBackground(void)
{
    /* Sky */
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, SCREEN_W - 1, GROUND_Y - 1);

    /* Sun — dim white (overcast) */
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(580, 55, 28, 28);

    /* Cloud 1 — left bank */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    fillellipse( 80, 85, 38, 20);
    fillellipse(118, 72, 48, 26);
    fillellipse(158, 85, 38, 20);

    /* Cloud 2 — centre bank */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    fillellipse(300, 65, 42, 22);
    fillellipse(342, 52, 52, 28);
    fillellipse(384, 65, 42, 22);

    /* Cloud 3 — right bank (partially covers the sun) */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    fillellipse(530, 58, 38, 20);
    fillellipse(566, 46, 48, 26);
    fillellipse(606, 58, 38, 20);

    /* Road surface */
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(0, GROUND_Y, SCREEN_W - 1, SCREEN_H - 1);

    /* Road edge lines */
    setcolor(LIGHTGRAY);
    line(0, GROUND_Y,     SCREEN_W - 1, GROUND_Y);
    line(0, SCREEN_H - 2, SCREEN_W - 1, SCREEN_H - 2);

    /* Road centre divider */
    setcolor(WHITE);
    line(0, 440, SCREEN_W - 1, 440);
}

/* ============================================================
 * drawRain
 *   Draws diagonal rain streaks tiled across the sky.
 *   'offset' advances each frame to animate falling motion.
 *
 *   Horizontal tiling : col - (offset % RAIN_COL_GAP)
 *   Vertical  tiling  : (row + offset) % GROUND_Y
 *   Each streak is a short diagonal line (4px left, 12px down).
 * ============================================================ */
void drawRain(int offset)
{
    int col, row, rx, ry;
    setcolor(LIGHTBLUE);

    for (row = 0; row < GROUND_Y; row += RAIN_ROW_GAP)
    {
        for (col = 0; col < SCREEN_W + RAIN_COL_GAP; col += RAIN_COL_GAP)
        {
            rx = col - (offset % RAIN_COL_GAP);
            ry = (row + offset) % GROUND_Y;
            line(rx, ry, rx - 4, ry + 12);
        }
    }
}

/* ============================================================
 * drawMan
 *   Draws the stick-figure man with umbrella.
 *   (mx, my) is the shoulder / torso-top position.
 *   legPhase (0 or 1) alternates to simulate walking.
 *
 *   Draw order: umbrella first, then body on top, so the
 *   umbrella handle appears held by the man's hand.
 * ============================================================ */
void drawMan(int mx, int my, int legPhase)
{
    /* --- Umbrella --- */
    /* Handle: from hand position up above head */
    setcolor(BROWN);
    line(mx - 14, my + 18, mx - 14, my - 22);

    /* Canopy fill */
    setcolor(RED);
    setfillstyle(SOLID_FILL, RED);
    fillellipse(mx - 14, my - 22, 28, 10);

    /* Canopy outline rim and base edge */
    setcolor(DARKGRAY);
    arc (mx - 14, my - 22, 0, 180, 28);
    line(mx - 42, my - 22, mx + 14, my - 22);

    /* --- Body --- */
    setcolor(WHITE);

    /* Head */
    circle(mx, my - 14, 10);

    /* Torso */
    line(mx, my - 4, mx, my + 30);

    /* Left arm — raises to hold umbrella handle */
    line(mx, my + 8, mx - 14, my + 18);

    /* Right arm — swings with opposite leg phase */
    if (legPhase == 0)
        line(mx, my + 8, mx + 16, my + 22);
    else
        line(mx, my + 8, mx + 10, my + 26);

    /* Legs — 2-phase walking cycle */
    if (legPhase == 0)
    {
        line(mx, my + 30, mx - 14, my + 58);  /* left forward  */
        line(mx, my + 30, mx + 10, my + 58);  /* right back    */
    }
    else
    {
        line(mx, my + 30, mx - 8,  my + 58);  /* left back     */
        line(mx, my + 30, mx + 14, my + 58);  /* right forward */
    }
}

/* ============================================================
 * main
 *   Initialises graphics, then runs the animation loop until
 *   a key is pressed.
 *
 *   Double-buffer pattern (per frame):
 *     1. setactivepage(nextPage) — point drawing to back buffer
 *     2. cleardevice()           — erase last frame on that page
 *     3. draw everything         — render full scene
 *     4. setvisualpage(nextPage) — flip: show the finished frame
 *     5. toggle nextPage         — swap buffers for next frame
 *
 *   All variable declarations appear BEFORE any executable
 *   statement, as required by C89 (Turbo C).
 * ============================================================ */
int main(void)
{
    /* All declarations MUST precede any statement in C89 */
    int manX     = MAN_START_X;
    int manY     = MAN_Y;
    int rainOff  = 0;
    int legPhase = 0;
    int frame    = 0;      /* bounded modulo LEG_TOGGLE_FRAMES */
    int page     = 0;      /* active drawing page (0 or 1)     */

    initCanvas();

    while (!kbhit())
    {
        /* Draw to the back page, then flip — eliminates flicker */
        page = 1 - page;
        setactivepage(page);
        cleardevice();

        drawBackground();
        drawRain(rainOff);
        drawMan(manX, manY, legPhase);

        setvisualpage(page);   /* atomic display swap */

        /* Advance animation state */
        manX    -= MAN_STEP;
        rainOff  = (rainOff + RAIN_SPEED) % (RAIN_ROW_GAP * RAIN_COL_GAP);

        /* Toggle leg phase every LEG_TOGGLE_FRAMES frames.
         * frame is bounded to prevent int overflow on long runs. */
        frame = (frame + 1) % (LEG_TOGGLE_FRAMES * 2);
        if (frame % LEG_TOGGLE_FRAMES == 0)
            legPhase = 1 - legPhase;

        /* Wrap man from left exit back to right entry */
        if (manX < MAN_WRAP_EXIT)
            manX = MAN_WRAP_ENTRY;

        delay(FRAME_DELAY_MS);
    }

    getch();
    closegraph();
    return 0;
}
