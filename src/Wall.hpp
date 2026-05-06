#ifndef WALL_HPP
#define WALL_HPP

#include <unordered_set>
#include <vector>
using namespace std;

struct pairHash{
    size_t operator()(const pair<int,int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

class Wall{
private:
unordered_set<pair<int,int> , pairHash> walls;
pair<int,int> space;

public:
Wall(const vector<pair<int,int>> &w, const pair<int,int> &sp);
bool isWall(const pair<int,int> &pos) const;
bool isValid(const pair<int,int> &pos) const;
const unordered_set<pair<int,int>, pairHash> &getWalls() const;  
};

#endif