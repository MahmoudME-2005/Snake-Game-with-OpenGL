#pragma once

/*
===============================================================================
File: Fruit.hpp
Changes from original:
 - Replaced `#include "Snake.hpp"` with `#include "Common.hpp"`.
   Fruit only needed PairHash from Snake.hpp, which now lives in Common.hpp.
   This breaks the circular-ish dependency (Fruit.hpp -> Snake.hpp -> ...).
 - Removed the forward declaration `struct PairHash;` — it was redundant since
   the full definition is now included via Common.hpp.
 - Removed duplicate standard library includes that Common.hpp already covers.
===============================================================================
*/

#include "../Common.hpp"
#include <unordered_set>
#include <utility>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <vector>

class Fruit
{
public:
    Fruit(int gridW, int gridH,
          const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells);

    void respawn(int gridW, int gridH,
                 const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells);

    int getX() const;
    int getY() const;

private:
    int x = 0, y = 0;

    void place(int gridW, int gridH,
               const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells);
};
