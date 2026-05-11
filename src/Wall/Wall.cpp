#include "Wall.hpp"

Wall::Wall(const vector<pair<int,int>> &w, const pair<int,int> &sp)
{
    this->space = sp;

    for(auto& p : w)
    {
        if(isValid(p))
        {
            this->walls.insert(p);
        }
        else
        {
            cerr << "position (" << p.first <<"," << p.second << ")" << " is invalid\n";
        }
    }   
}

// (for the snake) checks if the next position is a wall or not
bool Wall::isWall(const pair<int,int> &pos) const
{
    return this->walls.find(pos) != this->walls.end();
}

//checks if this position is valid for a new wall
bool Wall::isValid(const pair<int,int> &pos) const
{
    if(pos.first < 0 || pos.first >= this->space.first) return false;
    if(pos.second < 0 || pos.second >= this->space.second) return false;
    return !isWall(pos);
}

// for the gui
const unordered_set<pair<int,int>, pairHash>& Wall::getWalls() const
{
    return this->walls;
}

