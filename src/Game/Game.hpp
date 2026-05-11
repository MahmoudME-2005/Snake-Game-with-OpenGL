#pragma once

/*
===============================================================================
File: Game.hpp
Role: Central game-state class. Owns Snake, Fruit, and Wall.
      Drives the update loop: move, collision checks, scoring, state changes.

What Renderer expects from this class (all must be const):
    getScore()    -> int
    getHiScore()  -> int
    getState()    -> GameState
    getSnake()    -> const Snake&
    getFruit()    -> const Fruit&
    getWall()     -> const Wall&

What main.cpp uses:
    update()          called every timer tick when PLAYING
    handleKey(char)   for regular keys: P (pause), R (restart), ESC (quit)
    handleArrow(int)  for GLUT special keys: arrow keys -> snake direction
    reset()           rebuilds snake/fruit to start a new game
===============================================================================
*/

#include "../Common.hpp"
#include "../Snake/Snake.hpp"
#include "../Fruit/Fruit.hpp"
#include "../Wall/Wall.hpp"

class Game
{
public:
    // Builds the initial game state.
    // wallCells: list of (col,row) positions that are static walls.
    // Pass an empty vector for a wall-free game.
    Game(const std::vector<std::pair<int,int>>& wallCells = {});

    // ── Called by main.cpp every timer tick ─────────────────────────────
    // Advances the snake, checks all collisions, updates score and state.
    // Does nothing if state is not PLAYING.
    void update();

    // ── Input handlers (called from GLUT callbacks in main.cpp) ─────────
    // Regular ASCII keys: P = pause/unpause, R = restart, ESC = quit
    void handleKey(unsigned char key);
    // GLUT special keys: GLUT_KEY_UP / _DOWN / _LEFT / _RIGHT
    void handleArrow(int key);

    // Resets score and rebuilds snake + fruit. Keeps the same walls.
    void reset();

    // ── Read-only accessors used by Renderer ─────────────────────────────
    int            getScore()   const { return score;   }
    int            getHiScore() const { return hiScore; }
    GameState      getState()   const { return state;   }
    const Snake&   getSnake()   const { return snake;   }
    const Fruit&   getFruit()   const { return fruit;   }
    const Wall&    getWall()    const { return wall;     }

private:
    Snake     snake;
    Fruit     fruit;
    Wall      wall;

    int       score   = 0;
    int       hiScore = 0;
    GameState state   = GameState::PLAYING;

    // Builds the set of occupied cells (snake body + walls) used by Fruit::place.
    std::unordered_set<std::pair<int,int>, PairHash> buildOccupied() const;
};
