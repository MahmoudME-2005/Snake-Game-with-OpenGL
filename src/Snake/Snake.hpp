#pragma once

/*
===============================================================================
File: Snake.hpp
Changes from original:
 - Removed local PairHash definition. It now lives in Common.hpp so Snake,
   Wall, and Fruit all use the exact same type. This fixes the type mismatch
   when sets are passed between these classes.
 - Added #include "Common.hpp" to pull in PairHash, constants, and GameState.
 - Removed `using namespace std;` from the header — it pollutes every file
   that includes Snake.hpp. std:: is written explicitly instead.
===============================================================================
*/

#include "../Common.hpp"   // PairHash, GRID_W/H, WIN_W/H, GameState
#include <deque>
#include <unordered_set>
#include <utility>
#include <iostream>

using namespace std;

enum class Direction { UP, DOWN, LEFT, RIGHT };

class Snake
{
public:
    Snake(int startX, int startY);

    bool move();
    void grow();
    void setDirection(Direction newDir);

    bool occupies(int x, int y) const;

    std::pair<int,int>                                    getHead() const;
    const std::deque<std::pair<int,int>>&                 getBody() const;

    // Expose the body set so Game::buildOccupied() can include snake cells
    // when placing the fruit without iterating the deque separately.
    const std::unordered_set<std::pair<int,int>, PairHash>& getBodySet() const
    { return bodySet; }

private:
    std::deque<std::pair<int,int>>                        body;
    std::unordered_set<std::pair<int,int>, PairHash>      bodySet;
    Direction                                             direction;
    bool                                                  pendingGrow;

    std::pair<int,int> nextHeadPosition() const;
};
