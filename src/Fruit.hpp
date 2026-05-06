#pragma once
#include <unordered_set>
#include <utility>

// Reuse PairHash from Snake.h
struct PairHash;   // forward declaration from Snake.h

class Fruit {
public:
    // Spawns the fruit at a random position that avoids occupied cells.
    // occupiedCells should contain every grid position taken by the snake body and walls.
    Fruit(int gridW, int gridH,
          const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells);

    // Picks a new random position, again avoiding occupiedCells.
    // Call this immediately after the snake eats the fruit.
    void respawn(int gridW, int gridH,
                 const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells);

    int getX() const;
    int getY() const;

private:
    int x, y;

    // Shared placement logic used by both constructor and respawn().
    void place(int gridW, int gridH,
               const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells);
};
