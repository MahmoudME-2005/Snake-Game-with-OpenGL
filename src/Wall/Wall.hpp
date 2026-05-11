#pragma once

/*
===============================================================================
File: Wall.hpp
Changes from original:
 - Removed local `pairHash` struct. Now uses PairHash from Common.hpp.
   This was causing a type mismatch: Wall returned an unordered_set<PairHash>
   while Renderer and Fruit expected unordered_set<PairHash> from Snake.hpp.
   Now all three files use the same type from Common.hpp.
 - Removed `using namespace std;` from the header.
===============================================================================
*/

#include "../Common.hpp"
#include <unordered_set>
#include <vector>
#include <iostream>

using namespace std;

class Wall
{
public:
    Wall(const std::vector<std::pair<int,int>>& w,
         const std::pair<int,int>& sp);

    bool isWall(const std::pair<int,int>& pos) const;
    bool isValid(const std::pair<int,int>& pos) const;

    const std::unordered_set<std::pair<int,int>, PairHash>& getWalls() const;

private:
    std::unordered_set<std::pair<int,int>, PairHash> walls;
    std::pair<int,int>                               space;
};
