#include "Common.hpp"
#include "Game/Game.hpp"
#include "Renderer/Renderer.hpp"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glew.h>
#  include <GL/glut.h>
#endif

/*
===============================================================================
File: main.cpp
Role: Entry point. Owns the GLUT window, the Game object, and the Renderer.
      Registers all GLUT callbacks and starts the event loop.

Responsibilities:
  - Create the GLUT window and OpenGL context.
  - Create a Game and a Renderer.
  - Register GLUT callbacks:
      display  -> renderer draws the current game state
      reshape  -> renderer updates the viewport
      timer    -> game advances one tick, then requests a redraw
      keyboard -> forwarded to game.handleKey()
      special  -> forwarded to game.handleArrow()  (arrow keys)
===============================================================================
*/

// Global objects. GLUT callbacks are plain C functions so these must be
// accessible at file scope. Using globals here is the standard GLUT pattern.
static Game g_game({
    // ── Border ───────────────────────────────────────
    // Top and bottom rows
    {0,0},  {1,0},  {2,0},  {3,0},  {4,0},  {5,0},  {6,0},  {7,0},  {8,0},  {9,0},
    {10,0}, {11,0}, {12,0}, {13,0}, {14,0}, {15,0}, {16,0}, {17,0}, {18,0}, {19,0},
    {20,0}, {21,0}, {22,0}, {23,0}, {24,0}, {25,0}, {26,0}, {27,0}, {28,0}, {29,0},

    {0,29}, {1,29}, {2,29}, {3,29}, {4,29}, {5,29}, {6,29}, {7,29}, {8,29}, {9,29},
    {10,29},{11,29},{12,29},{13,29},{14,29},{15,29},{16,29},{17,29},{18,29},{19,29},
    {20,29},{21,29},{22,29},{23,29},{24,29},{25,29},{26,29},{27,29},{28,29},{29,29},

    // Left and right columns (excluding corners already added)
    {0,1},  {0,2},  {0,3},  {0,4},  {0,5},  {0,6},  {0,7},  {0,8},  {0,9},  {0,10},
    {0,11}, {0,12}, {0,13}, {0,14}, {0,15}, {0,16}, {0,17}, {0,18}, {0,19}, {0,20},
    {0,21}, {0,22}, {0,23}, {0,24}, {0,25}, {0,26}, {0,27}, {0,28},

    {29,1},  {29,2},  {29,3},  {29,4},  {29,5},  {29,6},  {29,7},  {29,8},  {29,9},  {29,10},
    {29,11}, {29,12}, {29,13}, {29,14}, {29,15}, {29,16}, {29,17}, {29,18}, {29,19}, {29,20},
    {29,21}, {29,22}, {29,23}, {29,24}, {29,25}, {29,26}, {29,27}, {29,28},

    // ── Top-left corner decoration ───────────────────
    {3,3}, {4,3}, {5,3},
    {3,4},
    {3,5},

    // ── Top-right corner decoration ──────────────────
    {24,3}, {25,3}, {26,3},
             {26,4},
             {26,5},

    // ── Bottom-left corner decoration ────────────────
    {3,24},
    {3,25},
    {3,26}, {4,26}, {5,26},

    // ── Bottom-right corner decoration ───────────────
             {26,24},
             {26,25},
    {24,26}, {25,26}, {26,26},

    // ── Center diamond ───────────────────────────────
    {14,12},
    {13,13}, {15,13},
    {14,14},

    {15,15},
    {14,16}, {16,16},
    {15,17},

    // ── Left and right mid barriers ──────────────────
    {7,12}, {7,13}, {7,14}, {7,15}, {7,16},
    {22,12},{22,13},{22,14},{22,15},{22,16},

    // ── Top and bottom mid barriers ──────────────────
    {12,6}, {13,6}, {14,6}, {15,6}, {16,6},
    {12,23},{13,23},{14,23},{15,23},{16,23},
});

static Renderer g_renderer;

// ── GLUT callback: draw one frame ────────────────────────────────────────────
void display()
{
    g_renderer.draw(g_game);
}

// ── GLUT callback: window resized ────────────────────────────────────────────
void reshape(int w, int h)
{
    g_renderer.reshape(w, h);
}

// ── GLUT callback: regular keys (ASCII) ──────────────────────────────────────
void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    g_game.handleKey(key);
    glutPostRedisplay();   // redraw immediately so pause overlay appears at once
}

// ── GLUT callback: special keys (arrows, F-keys) ─────────────────────────────
void specialKey(int key, int /*x*/, int /*y*/)
{
    g_game.handleArrow(key);
}

// ── GLUT callback: timer tick ────────────────────────────────────────────────
// Called every TICK_MS milliseconds.
// Advances game logic then schedules the next tick and a redraw.
static constexpr int TICK_MS = 150;   // snake speed; lower = faster

void timerTick(int /*value*/)
{
    g_game.update();
    glutPostRedisplay();
    glutTimerFunc(TICK_MS, timerTick, 0);  // re-arm the timer
}

// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char** argv)
{
    // ── GLUT window setup ────────────────────────────────────────────────
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  // double-buffered, no depth needed
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Snake");

    // ── Renderer setup (must be after window/context creation) ───────────
    g_renderer.init("shaders/vertex.glsl", "shaders/fragment.glsl");

    // ── Register GLUT callbacks ──────────────────────────────────────────
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(TICK_MS, timerTick, 0);  // start the game tick timer

    // ── Enter GLUT event loop (never returns) ────────────────────────────
    glutMainLoop();
    return 0;
}
