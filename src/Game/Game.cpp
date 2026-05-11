#include "Game.hpp"

Game::Game(Snake snake, Fruit fruit, Wall wall)
{
    this->snake = snake;
    this->fruit = fruit;
    this->wall = wall;
}