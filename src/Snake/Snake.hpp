#pragma once
#include <deque>
#include <unordered_set>
#include <utility>
using namespace std;

// Custom hasher — needed because STL has no default hash for pair<int,int>
//we want to do unordered-set(template library stores unique elements) to prevent collisions (bit shift <<16)
struct PairHash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 16);
    }
};

enum class Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
public:
    Snake(int startX, int startY); //done

    void move(); //done
    void grow(); //done
    void setDirection(Direction newDir); //done

    bool checkSelfCollision() const; //done
	bool occupies(int x, int y) const;    // O(1) via unordered_set //done

    pair<int, int> getHead() const; //done
    const deque<pair<int, int>>& getBody() const;//done

private:
    deque<pair<int, int>>                      body; //Track the order of the Snake segmgent ,that moving by adding a head and remove a tail
   unordered_set<pair<int, int>, PairHash>    bodySet; // track the existence of the segment 
    Direction                                            direction;
    bool                                                 pendingGrow;

    pair<int, int> nextHeadPosition() const; //done
};

