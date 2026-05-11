#include "Game.hpp"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

/*
===============================================================================
File: Game.cpp
Role: Implements the game update loop, collision detection, scoring,
      state transitions, and input routing.

What this file does NOT do:
- Draw anything (that is Renderer's job).
- Read raw keyboard events (main.cpp registers GLUT callbacks and calls
  handleKey / handleArrow here).
===============================================================================
*/

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
//  wallCells: optional list of (col,row) positions to place static walls.
// ─────────────────────────────────────────────────────────────────────────────
Game::Game(const std::vector<std::pair<int,int>>& wallCells)
    : snake(GRID_W / 2, GRID_H / 2)                         // start snake in the centre
    , wall(wallCells, {GRID_W, GRID_H})                      // build wall set
    , fruit(GRID_W, GRID_H, buildOccupied())                 // place fruit on a free cell
{
}

// ─────────────────────────────────────────────────────────────────────────────
//  buildOccupied()
//  Combines snake body and wall positions into one set so Fruit::place()
//  knows which cells are unavailable.
// ─────────────────────────────────────────────────────────────────────────────
std::unordered_set<std::pair<int,int>, PairHash> Game::buildOccupied() const
{
    std::unordered_set<std::pair<int,int>, PairHash> occ;

    // Every snake segment is occupied
    for (auto& seg : snake.getBody())
    {
        occ.insert(seg);
    }

    // Every wall cell is occupied
    for (auto& w : wall.getWalls())
    {
        occ.insert(w);
    }

    return occ;
}

// ─────────────────────────────────────────────────────────────────────────────
//  update()
//  Called every timer tick. Moves the snake then checks all collisions.
// ─────────────────────────────────────────────────────────────────────────────
void Game::update()
{
    if (state != GameState::PLAYING) return;

    if(!snake.move())
    {
        state = GameState::GAME_OVER;
        return;
    }

    auto head = snake.getHead();

    // ── Wall collision ───────────────────────────────────────────────────
    if (wall.isWall(head))
    {
        state = GameState::GAME_OVER;
        return;
    }

    // ── Fruit eaten ──────────────────────────────────────────────────────
    if (head.first == fruit.getX() && head.second == fruit.getY())
    {
        snake.grow();
        score += 1;
        if (score > hiScore) hiScore = score;

        fruit.respawn(GRID_W, GRID_H, buildOccupied());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  handleKey()
//  Handles regular ASCII keys forwarded from glutKeyboardFunc in main.cpp.
// ─────────────────────────────────────────────────────────────────────────────
void Game::handleKey(unsigned char key)
{
    switch (key)
    {
        case 'p':
        case 'P':
            if (state == GameState::PLAYING)
                state = GameState::PAUSED;
            else if (state == GameState::PAUSED)
                state = GameState::PLAYING;
            break;

        case 'r':
        case 'R':
            reset();
            break;

        case 27: // ESC
            std::exit(0);
            break;

        default:
            break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  handleArrow()
//  Handles GLUT special keys forwarded from glutSpecialFunc in main.cpp.
//  Snake::setDirection() already blocks 180-degree reversals internally.
// ─────────────────────────────────────────────────────────────────────────────
void Game::handleArrow(int key)
{
    // Arrow keys only steer the snake while the game is running
    if (state != GameState::PLAYING) return;

    switch (key)
    {
        case GLUT_KEY_UP:    snake.setDirection(Direction::UP);    break;
        case GLUT_KEY_DOWN:  snake.setDirection(Direction::DOWN);  break;
        case GLUT_KEY_LEFT:  snake.setDirection(Direction::LEFT);  break;
        case GLUT_KEY_RIGHT: snake.setDirection(Direction::RIGHT); break;
        default: break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  reset()
//  Starts a new game. Hi-score is preserved across resets.
// ─────────────────────────────────────────────────────────────────────────────
void Game::reset()
{
    score = 0;
    state = GameState::PLAYING;
    snake = Snake(GRID_W / 2, GRID_H / 2);
    fruit.respawn(GRID_W, GRID_H, buildOccupied());
}
