#pragma once

#include "../Snake/Snake.hpp"
#include "../Fruit/Fruit.hpp"
#include "../Wall/Wall.hpp"

class Game
{
    private:
    Snake snake;
    Fruit fruit;
    Wall wall;
    public:
    Game(Snake snake, Fruit fruit, Wall wall);
};