#pragma once
#include "../Game/Game.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
/*
===============================================================================
File: Renderer.h
Role: Public interface for the OpenGL renderer used by the Snake game.

What this file does:
- Declares the Renderer class.
- Defines the functions that the main/game loop can call to initialize OpenGL
    drawing, draw each game frame, and handle window resizing.
- Keeps OpenGL implementation details private inside Renderer.cpp.

What this file expects from other parts of the project:
- Game.h must define the Game class.
- The Game class must provide the data needed for drawing, such as:
    getScore()
    getHiScore()
    getState()
    getSnake()
    getFruit()
    getWall()
- Game.h or related shared headers must also provide game constants/enums used
    by the renderer implementation, such as WIN_W, WIN_H, GRID_W, GRID_H,
    CELL, HUD_H, and GameState.

What other files can use from this file:
- main.cpp / Game.cpp can create a Renderer object.
- They can call:
    init(...)      once after the OpenGL/GLUT window is created
    draw(game)     every frame
    reshape(w, h)  whenever the window is resized
===============================================================================
*/

class Renderer
{
public:
    // Load & compile shaders, set up GL state.
    // Call once after the GLUT window is created.
        /*
    Initializes the renderer.

    Expected to be called once after the OpenGL window/context is created.
    It tries to:
    1. Initialize GLEW on non-Apple platforms.
    2. Load the vertex and fragment shader files.
    3. Compile and link the shader program.
    4. Create a reusable quad VAO/VBO for drawing grid cells.

    If shader loading/compilation fails, shaderProgram stays 0, and the renderer
    will use drawFallback() instead of shader-based drawing.
    */
    void init(const std::string& vertPath, const std::string& fragPath);
    /*
    Draws one complete frame of the game.

    Called by the main/game loop every frame. It draws:
    - background
    - grid
    - walls
    - fruit
    - snake
    - HUD score bar
    - pause/game-over overlay when needed

    The function reads game data only; it does not update game logic.
    */
    void draw(const Game& game);

    // Called on window resize.
    void reshape(int w, int h);

private:
// ── Shader program ──────────────────────────────────────────────────

    // OpenGL shader program ID. If it is 0, shader failed / not loaded
    unsigned int shaderProgram = 0;  

// ── Uniform locations ──────────────────────────────────────────────

    // These are filled after successful shader linking in init().
    int uColor    = -1;  // colour of the cell/shape being drawn
    int uOffset   = -1;  // pixel position where the unit quad should be drawn
    int uCellSize = -1;  // pixel size of the quad after scaling

// ── Quad VAO/VBO  ───────────────

    // Reusable geometry for drawing cells.
    // The quad is a unit square; the shader scales and moves it per cell.
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

// ── Internal helpers ────────────────────────────────────────────────

    // Compiles one shader object from source code and returns its OpenGL ID.
    // Returns 0 if compilation fails.
    unsigned int compileShader(unsigned int type, const std::string& src);

    // Loads a text file from disk, used for shader files.
    // Returns an empty string if the file cannot be opened.
    std::string  loadFile(const std::string& path);

    // Backup renderer using old fixed-function OpenGL.
    // Used when shader initialization fails so the game can still be displayed.
    void drawFallback(const Game& game);

    // Draws a single grid cell using the shader pipeline.
    // col/row are grid coordinates, not pixels.
    void drawCellShader(int col, int row, float r, float g, float b, int inset = 2);
    // Draws the top HUD bar: score, best score, pause text.
    void drawHUD(const Game& game);
    // Draws pause/game-over overlay when the game is not in PLAYING state
    void drawOverlay(const Game& game);

    // Draws simple bitmap text using GLUT fixed-function text rendering. (always uses fixed-function pipeline for simplicity)
    // This avoids adding a separate font/text rendering library.
    static void drawText(int px, int py, const std::string& s, void* font = nullptr);
};