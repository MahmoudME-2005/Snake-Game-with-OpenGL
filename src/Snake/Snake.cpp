#include "Snake.hpp"
#include <iostream>
using namespace std;
Snake::Snake(int startX, int startY) {
	direc= Direction::RIGHT; //as default value canbe changed 
	pendingGrow =false; //as default value too
	body.push_front({startX, startY});
    bodySet.insert({startX, startY});
}


//no need to explain 
pair<int, int> Snake::getHead() const {
	return body.front();

}
const deque<pair<int, int>>& Snake::getBody() const {
	return body;
}


pair<int, int> Snake::nextHeadPosition() const { //build it first as we need it at the move method
	//we fisrt look at the front as we need to add heads
	pair<int, int> temp = body.front();
	// for simplicity  @ the look dataType ref ,i will use "auto" keyword as it automatically bring the dataType 


	//ask for the snake direction

	

}
