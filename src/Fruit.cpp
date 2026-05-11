#include "Fruit.hpp"

//Instead of generating random positions repeatedly it scans the entire grid and collect empty cells than randomly choose one of them

//generate a valid fruit position.
void Fruit::place(int gridW, int gridH, const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells) //Contains all snake body positions
{
    std::vector<std::pair<int,int>> freeCells; //This vector will contain every empty position

    //Reserve space to avoid resizing the vector many times "just for memory no elemnts are added"
    freeCells.reserve(gridW * gridH - static_cast<int>(occupiedCells.size()));

    //Loop on every cell in the grid
    for (int col = 0; col < gridW; ++col)
    {
        for (int row = 0; row < gridH; ++row)
        {
            //If the cell is not occupied by the snake add it to freeCells
            if (occupiedCells.find({col, row}) == occupiedCells.end())
            {
                freeCells.push_back({col, row}); //end iterator
            }
        }
    }

    //If there are no free cells then the whole grid is full
    if (freeCells.empty())
    {
        throw std::runtime_error("Fruit::place — no free cell available (grid is full)");
    }

    //Pick a random free cell
    int idx = rand() % static_cast<int>(freeCells.size()); //cause rand() generates big integers that may be larger than our grid size
    //used static cast int to inforce int valuse only cause mod may return double

    //Store the fruit position
    x = freeCells[idx].first;
    y = freeCells[idx].second;
}

//Constructor >> Runs automatically when object is created
Fruit::Fruit(int gridW, int gridH, const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells)
{
    //Seed the random generator
    //Initialize the random generator using the current time so results differ every run
    //without it will always start from the same position
    srand(static_cast<unsigned>(time(nullptr)));

    //Place the fruit in a valid empty cell
    place(gridW, gridH, occupiedCells);
}

//Called when the snake eats the fruit
void Fruit::respawn(int gridW, int gridH, const std::unordered_set<std::pair<int,int>, PairHash>& occupiedCells)
{
    //Generate a new fruit position
    place(gridW, gridH, occupiedCells);
}

//Return x position of the fruit
int Fruit::getX() const { return x; }

//Return y position of the fruit
int Fruit::getY() const { return y; }