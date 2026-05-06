#include "Fruit.h"
#include "Snake.h"   // brings in PairHash
#include <stdexcept>
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <vector>

// ──────────────────────────────────────────────────────────────────────────────
// Internal helper: collect every free cell and pick one at random.
//
// Why a vector of free cells instead of retrying random positions?
//   - Retry loops can spin forever when the grid is nearly full (e.g. the snake
//     fills 95 % of the board).  Building the free-cell list is O(gridW * gridH)
//     but guaranteed to terminate, and it gives a perfectly uniform distribution.
// ──────────────────────────────────────────────────────────────────────────────
void Fruit::place(int gridW, int gridH,
                  const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells)
{
    std::vector<std::pair<int,int>> freeCells;
    freeCells.reserve(gridW * gridH - static_cast<int>(occupiedCells.size()));

    for (int col = 0; col < gridW; ++col)
        for (int row = 0; row < gridH; ++row)
            if (occupiedCells.find({col, row}) == occupiedCells.end())
                freeCells.push_back({col, row});

    if (freeCells.empty())
        throw std::runtime_error("Fruit::place — no free cell available (grid is full)");

    int idx = rand() % static_cast<int>(freeCells.size());
    x = freeCells[idx].first;
    y = freeCells[idx].second;
}

// ──────────────────────────────────────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────────────────────────────────────
Fruit::Fruit(int gridW, int gridH,
             const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells)
{
    srand(static_cast<unsigned>(time(nullptr)));  // seed once at startup
    place(gridW, gridH, occupiedCells);
}

// ──────────────────────────────────────────────────────────────────────────────
// respawn — call after the snake eats the fruit
// ──────────────────────────────────────────────────────────────────────────────
void Fruit::respawn(int gridW, int gridH,
                    const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells)
{
    place(gridW, gridH, occupiedCells);
}

int Fruit::getX() const { return x; }
int Fruit::getY() const { return y; }
