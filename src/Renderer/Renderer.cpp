#include "Renderer.hpp"

#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glew.h>
#  include <GL/glut.h>
#endif
/*
===============================================================================
File: Renderer.cpp
Role: OpenGL drawing implementation for the Snake game.

What this file does:
- Loads and compiles GLSL shaders.
- Creates reusable quad geometry for drawing snake/fruit/wall cells.
- Draws the game grid, snake body, snake head, fruit, walls, HUD, and overlays.
- Provides a fallback fixed-function renderer if shaders are unavailable.

What this file expects from other project files:
- Renderer.h includes Game.h.
- Game must provide read-only access to the current game state:
    game.getScore()
    game.getHiScore()
    game.getState()
    game.getSnake().getBody()
    game.getFruit().getX() / getY()
    game.getWall().getWalls()
- Game constants/enums must exist:
    WIN_W, WIN_H, GRID_W, GRID_H, CELL, HUD_H, GameState::PLAYING,
    GameState::PAUSED, GameState::GAME_OVER
- Shader files must exist at the paths passed to Renderer::init(). Usually:
    shaders/vertex.glsl
    shaders/fragment.glsl

What other files can use from this file:
- main.cpp / Game.cpp should not call private helpers here directly.
- They should use only the public Renderer functions declared in Renderer.h:
    init(...), draw(game), reshape(w, h)
===============================================================================
*/



// ─── Theme colours ────────────────────────────────────────────────────────


// Small RGB colour container used to keep the theme colours readable
struct C3 { float r, g, b; };

// Centralized theme colours for the renderer
// Changing these values updates the visual style without touching draw logic
static constexpr C3 COL_BG      = {0.04f, 0.04f, 0.04f};
static constexpr C3 COL_GRID    = {0.10f, 0.10f, 0.10f};
static constexpr C3 COL_SNAKE_H = {0.40f, 1.00f, 0.30f};
static constexpr C3 COL_SNAKE_B = {0.20f, 0.75f, 0.15f};
static constexpr C3 COL_FRUIT   = {0.90f, 0.15f, 0.15f};
static constexpr C3 COL_WALL    = {0.30f, 0.30f, 0.30f};
static constexpr C3 COL_TEXT    = {0.85f, 0.85f, 0.85f};
static constexpr C3 COL_OVER    = {0.90f, 0.20f, 0.20f};
static constexpr C3 COL_HUD_BG  = {0.08f, 0.08f, 0.08f};

// ─────────────────────────────────────────────────────────────────────────────
//  File loader
// Reads a shader text file from disk and returns the full file content
// If the file cannot be opened, returns an empty string and logs the problem
// ─────────────────────────────────────────────────────────────────────────────
std::string Renderer::loadFile(const std::string& path)
{
    std::ifstream f(path);

    if (!f.is_open())
    {
        std::cerr << "[Renderer] Cannot open shader: " << path << "\n";
        return "";
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Shader compilation
// Compiles one GLSL shader object. The type is usually GL_VERTEX_SHADER or
// GL_FRAGMENT_SHADER. Returns 0 if compilation fails
// ─────────────────────────────────────────────────────────────────────────────
unsigned int Renderer::compileShader(unsigned int type, const std::string& src)
{
    unsigned int id = glCreateShader(type);
    const char* c   = src.c_str();
    glShaderSource(id, 1, &c, nullptr);
    glCompileShader(id);

    int ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "[Renderer] Shader compile error:\n" << log << "\n";
        glDeleteShader(id);
        return 0;
    }
    return id;
}

// ─────────────────────────────────────────────────────────────────────────────
//  init()  –  compile shaders, create quad VAO
// Main setup function for the renderer
// Must be called after a valid OpenGL/GLUT window/context already exists
// ─────────────────────────────────────────────────────────────────────────────
void Renderer::init(const std::string& vertPath, const std::string& fragPath)
{
#ifndef __APPLE__
    // GLEW loads modern OpenGL function pointers on Windows/Linux
    // Without this, functions such as glCreateShader may not be available
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "[Renderer] GLEW init failed – falling back to fixed pipeline.\n";
        return;
    }
#endif

    // Load shader source files from disk
    std::string vertSrc = loadFile(vertPath);
    std::string fragSrc = loadFile(fragPath);
    if (vertSrc.empty() || fragSrc.empty())
    {
        std::cerr << "[Renderer] Shader source missing – falling back to fixed pipeline.\n";
        return;
    }

        // Compile both shader stages
    unsigned int vert = compileShader(GL_VERTEX_SHADER,   vertSrc);
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!vert || !frag) return;

        // Link the vertex and fragment shaders into one shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);

    int ok;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cerr << "[Renderer] Program link error:\n" << log << "\n";
        shaderProgram = 0;
        return;
    }

        // Shader objects are no longer needed after linking
    glDeleteShader(vert);
    glDeleteShader(frag);

        // Cache uniform locations so drawCellShader() can update them quickly
    uColor    = glGetUniformLocation(shaderProgram, "uColor");
    uOffset   = glGetUniformLocation(shaderProgram, "uOffset");
    uCellSize = glGetUniformLocation(shaderProgram, "uCellSize");

    // Unit square vertex data
    // It is reused for every cell; position and size are controlled by uniforms
    float verts[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

        // Create and configure the VAO/VBO used for shader cell drawing
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1,      &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        // Attribute location 0 matches the vertex shader input: layout(location = 0) in vec2 aPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::cout << "[Renderer] Shaders loaded OK.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  reshape()
// Called when the window size changes. Currently only updates the viewport
// ─────────────────────────────────────────────────────────────────────────────
void Renderer::reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawCellShader() 
// Draws one logical grid cell using the shader pipeline
// col/row are grid coordinates; inset creates a small margin inside the cell
// ─────────────────────────────────────────────────────────────────────────────

void Renderer::drawCellShader(int col, int row, float r, float g, float b, int inset)
{
    // Convert grid coordinates to pixel coordinates
    // The game grid treats row 0 as the top row below the HUD
    // OpenGL pixel coordinates here are bottom-up, so the row is flipped
    float gridPixH = (float)(GRID_H * CELL);
    float totalH   = (float)WIN_H;

    float px = (float)(col * CELL + inset);
    // OpenGL y=0 is bottom; grid row 0 is at the top (just below HUD).
    float py = (float)(HUD_H + (GRID_H - row - 1) * CELL + inset);

    float sx = (float)(CELL - inset * 2);
    float sy = (float)(CELL - inset * 2);

    // Pass per-cell data to the shader
    glUniform3f(uColor, r, g, b);
    glUniform2f(uOffset, px, py);
    glUniform2f(uCellSize, sx, sy);

    // Draw the reusable unit quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawText()  
// Draws simple GLUT bitmap text in the current OpenGL projection
// Text drawing uses the fixed-function pipeline for simplicity
// ─────────────────────────────────────────────────────────────────────────────
void Renderer::drawText(int px, int py, const std::string& s, void* font)
{
    if (!font) font = GLUT_BITMAP_HELVETICA_18;
    glRasterPos2i(px, py);
    for (char ch : s) glutBitmapCharacter(font, ch);
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawHUD()
// Draws the top HUD strip with score, best score, and pause status text
// ─────────────────────────────────────────────────────────────────────────────
void Renderer::drawHUD(const Game& game)
{
    // Save the current matrices because this function temporarily switches to
    // a 2D pixel coordinate projection for easy HUD drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // HUD/text are drawn without the shader program
    glUseProgram(0);

    // Draw the HUD background strip at the top of the window
    glColor3f(COL_HUD_BG.r, COL_HUD_BG.g, COL_HUD_BG.b);
    glBegin(GL_QUADS);
    glVertex2i(0,     WIN_H - HUD_H);
    glVertex2i(WIN_W, WIN_H - HUD_H);
    glVertex2i(WIN_W, WIN_H);
    glVertex2i(0,     WIN_H);
    glEnd();

    // Draw score and game status text.=
    glColor3f(COL_TEXT.r, COL_TEXT.g, COL_TEXT.b);
    drawText(10,          WIN_H - HUD_H + 12, "SCORE: " + std::to_string(game.getScore()));
    drawText(WIN_W/2 - 50, WIN_H - HUD_H + 12, "BEST: "  + std::to_string(game.getHiScore()));
    drawText(WIN_W - 130,  WIN_H - HUD_H + 12,
            game.getState() == GameState::PAUSED ? "[PAUSED]" : "[P] Pause",
            GLUT_BITMAP_HELVETICA_12);

    // Restore matrices so later drawing is not affected
    glMatrixMode(GL_PROJECTION); 
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  
    glPopMatrix();
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawOverlay() 
// Draws the pause or game-over panel when the game is not actively playing
// ─────────────────────────────────────────────────────────────────────────────
void Renderer::drawOverlay(const Game& game)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glUseProgram(0);

    GameState st = game.getState();

    // Semi-transparent dark box behind the pause/game-over message
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.70f);
    glBegin(GL_QUADS);
    glVertex2i(WIN_W/4,     WIN_H/4);
    glVertex2i(3*WIN_W/4,   WIN_H/4);
    glVertex2i(3*WIN_W/4, 3*WIN_H/4);
    glVertex2i(WIN_W/4,   3*WIN_H/4);
    glEnd();
    glDisable(GL_BLEND);

    if (st == GameState::GAME_OVER)
    {
        glColor3f(COL_OVER.r, COL_OVER.g, COL_OVER.b);
        drawText(WIN_W/2 - 68, WIN_H/2 + 20, "GAME  OVER",
                GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(COL_TEXT.r, COL_TEXT.g, COL_TEXT.b);
        drawText(WIN_W/2 - 50, WIN_H/2 - 10,
                "Score: " + std::to_string(game.getScore()),
                GLUT_BITMAP_HELVETICA_18);
        drawText(WIN_W/2 - 90, WIN_H/2 - 40,
                "Press R to restart", GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        glColor3f(COL_SNAKE_H.r, COL_SNAKE_H.g, COL_SNAKE_H.b);
        drawText(WIN_W/2 - 42, WIN_H/2 + 10, "PAUSED",
                GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(COL_TEXT.r, COL_TEXT.g, COL_TEXT.b);
        drawText(WIN_W/2 - 78, WIN_H/2 - 20,
                "Press P to continue", GLUT_BITMAP_HELVETICA_12);
    }

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

// ─────────────────────────────────────────────────────────────────────────────
//  draw()  
// Public rendering entry point. Called once per frame by the game loop
// ─────────────────────────────────────────────────────────────────────────────
void Renderer::draw(const Game& game)
{
    glClearColor(COL_BG.r, COL_BG.g, COL_BG.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

        // If shader setup failed, use the simpler fallback path
    if (!shaderProgram) {
        drawFallback(game);
        glutSwapBuffers();
        return;
    }

    // ── Set up shader projection (NDC via uniform transforms) ────────────
        // Use the shader program for cell-based drawing
    glUseProgram(shaderProgram);

    // Pass the window size so vertex.glsl can convert pixels to NDC
    int uWinSize = glGetUniformLocation(shaderProgram, "uWinSize");
    glUniform2f(uWinSize, (float)WIN_W, (float)WIN_H);

    // ── Grid lines (fixed-function, thin) ──────────────────────────────
    // Draw grid lines with fixed-function OpenGL because they are simple lines
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glColor3f(COL_GRID.r, COL_GRID.g, COL_GRID.b);
    glBegin(GL_LINES);
    for (int c = 0; c <= GRID_W; ++c) {
        glVertex2i(c * CELL, HUD_H);
        glVertex2i(c * CELL, WIN_H);
    }
    for (int r = 0; r <= GRID_H; ++r) {
        glVertex2i(0,     HUD_H + r * CELL);
        glVertex2i(WIN_W, HUD_H + r * CELL);
    }
    glEnd();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();

    // ── Switch back to shader program to draw the game objects as filled cells ──────────────────────────────────
    glUseProgram(shaderProgram);

    // Draw static walls first so snake/fruit can appear on top 
    // Collision/spawn prevention is handled by Game/Fruit/Wall logic, not by Renderer.
    for (auto& p : game.getWall().getWalls())
        drawCellShader(p.first, p.second, COL_WALL.r, COL_WALL.g, COL_WALL.b, 1);

    // Draw fruit
    drawCellShader(game.getFruit().getX(), game.getFruit().getY(),
                COL_FRUIT.r, COL_FRUIT.g, COL_FRUIT.b, 3);

    // Draw snake. The first segment is treated as the head
    bool head = true;
    for (auto& seg : game.getSnake().getBody())
    {
        if (head)
            drawCellShader(seg.first, seg.second,
                        COL_SNAKE_H.r, COL_SNAKE_H.g, COL_SNAKE_H.b, 1);
        else
            drawCellShader(seg.first, seg.second,
                        COL_SNAKE_B.r, COL_SNAKE_B.g, COL_SNAKE_B.b, 2);
        head = false;
    }

    // HUD and overlay use fixed-function OpenGL text rendering
    glUseProgram(0);

    // ── HUD ──────────────────────────────────────────────────────────────
    drawHUD(game);

    // ── Overlay (pause / game-over) ───────────────────────────────────────
    if (game.getState() != GameState::PLAYING)
        drawOverlay(game);

    // Because GLUT_DOUBLE is expected, swap the back buffer to the screen
    glutSwapBuffers();
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawFallback()  –  classic fixed-function pipeline (no shaders needed)
// ─────────────────────────────────────────────────────────────────────────────

// Backup renderer that does not require GLSL shaders
// This helps the program still display something if shader loading fails
// -----------------------------------------------------------------------------
void Renderer::drawFallback(const Game& game)
{
// Use pixel coordinates for the whole window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Local helper for drawing a rectangle in pixel coordinates
    auto drawRect = [](int px, int py, int w, int h)
    {
        glBegin(GL_QUADS);
        glVertex2i(px,   py);   glVertex2i(px+w, py);
        glVertex2i(px+w, py+h); glVertex2i(px,   py+h);
        glEnd();
    };

    // Local helper that converts grid cell coordinates to pixel coordinates
    // row 0 is the top row of the playable grid area
    auto cellPx = [&](int col, int row, const C3& c, int inset)
    {
        int px = col * CELL + inset;
        int py = HUD_H + (GRID_H - row - 1) * CELL + inset;
        glColor3f(c.r, c.g, c.b);
        drawRect(px, py, CELL - inset*2, CELL - inset*2);
    };

    // Draw grid lines
    glColor3f(COL_GRID.r, COL_GRID.g, COL_GRID.b);
    glBegin(GL_LINES);
    for (int c = 0; c <= GRID_W; ++c) {
        glVertex2i(c*CELL, HUD_H); glVertex2i(c*CELL, WIN_H);
    }
    for (int r = 0; r <= GRID_H; ++r) {
        glVertex2i(0, HUD_H + r*CELL); glVertex2i(WIN_W, HUD_H + r*CELL);
    }
    glEnd();

    // Draw game objects
    for (auto& p : game.getWall().getWalls())  cellPx(p.first, p.second, COL_WALL, 1);
    cellPx(game.getFruit().getX(), game.getFruit().getY(), COL_FRUIT, 3);

    bool head = true;
    for (auto& seg : game.getSnake().getBody())
    {
        cellPx(seg.first, seg.second, head ? COL_SNAKE_H : COL_SNAKE_B, head ? 1 : 2);
        head = false;
    }

    drawHUD(game);
    if (game.getState() != GameState::PLAYING) drawOverlay(game);
}
