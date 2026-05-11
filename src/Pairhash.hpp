#include <utility>

// Custom hasher — needed because STL has no default hash for pair<int,int>
//we want to do unordered-set(template library stores unique elements) to prevent collisions (bit shift <<16)
struct PairHash
{
    size_t operator()(const pair<int, int>& p) const
    {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 16);
    }
};
