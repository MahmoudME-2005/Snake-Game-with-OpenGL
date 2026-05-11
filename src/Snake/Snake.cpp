#include "Snake.hpp"
#include <iostream>
using namespace std;
Snake::Snake(int startX, int startY) {
	direction= Direction::RIGHT; //as default value canbe changed 
	pendingGrow =false; //as default value too
	body.push_front({startX, startY});
    bodySet.insert({startX, startY});
}


//no need to explain 
//Simple getters
pair<int, int> Snake::getHead() const {
	return body.front();
}
const deque<pair<int, int>>& Snake::getBody() const {
	return body;
}


//helper method to calculate the next head position based on the current direction of the snake. This method is crucial for both moving and growing the snake, as it determines where the new head will be added.
pair<int, int> Snake::nextHeadPosition() const { //build it first as we need it at the move method
	//we fisrt look at the front as we need to add heads
	pair<int, int> temp = body.front();
		int x = temp.first;
		int y = temp.second;
	


	//ask for the snake direction
	//grabs the current head body.front and modify the X &Y based on the enum direction
	if (direction == Direction::UP)    return { x, y - 1 };
	if (direction == Direction::DOWN)  return { x, y + 1 };
	if (direction == Direction::LEFT)  return { x - 1, y };
	if (direction == Direction::RIGHT) return { x + 1, y };
	return { x, y };
}




//The logic of moving adding a head in the front of the body and renove from the tail
void Snake:: move() {
	pair<int, int>  newHead = nextHeadPosition();
	//Get the new head position and push it to the front of the body  deque and bodySet and also to the set for quick lookup
	body.push_front(newHead);
	bodySet.insert(newHead);


	//If the snake is not growing, we need to remove the tail segment to maintain the same length. If it is growing, we keep the tail and reset the pendingGrow flag for the next move.
	if (!pendingGrow) {
		bodySet.erase(body.back());
		body.pop_back();
	}
	else {
		pendingGrow = false;
	}
	
}



//The logic of growing is to set the pendingGrow flag to true, which will cause the next move to add a new head without removing the tail, effectively increasing the length of the snake by one segment.
// Setter method:)
void Snake::grow() {
		pendingGrow = true;
}



//if the Snake moving UP,it shouldn't allow to move down and so on for the other direction , this is to prevent the snake from reversing direction and colliding with itself immediately.
void Snake::setDirection(Direction newDir) {
    // Prevent the snake from reversing direction
    if ((direction == Direction::UP && newDir != Direction::DOWN) ||
        (direction == Direction::DOWN && newDir != Direction::UP) ||
        (direction == Direction::LEFT && newDir != Direction::RIGHT) ||
        (direction == Direction::RIGHT && newDir != Direction::LEFT)) {
        direction = newDir;
    }
	//:) , THX Git coplite for auto complete the code 
}




//checks if the nextHeadPosition() is already present in bodySet
//as the BodySet is unordered_set , it stores unique elements and allows for O(1) time complexity when checking for the presence of the head position. If the head position is found more than once in the bodySet, it indicates a collision with itself.
bool Snake::checkSelfCollision() const {
    // Check if the head collides with any other part of the body
    const pair<int, int>& head = body.front();
    return bodySet.count(head) > 1;
}



//it maybe forensure that the food is not placed on the snake body, we can use this method to check if the snake occupies a specific coordinate (x, y) before placing food there. By using an unordered_set (bodySet) to track the coordinates of the snake's body segments, we can efficiently check for occupancy in O(1) time complexity.
//The occupies method checks if the snake occupies a specific coordinate (x, y) by looking it up in the bodySet. This allows for O(1) time complexity when checking for collisions or food placement.
bool Snake::occupies(int x, int y) const {
    return bodySet.count({x, y}) > 0;
}