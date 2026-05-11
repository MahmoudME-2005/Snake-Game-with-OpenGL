//this main is just for test what i wote 
//it completly AI genenated and i just want to test it
//:)
#include <iostream>
#include <vector>
#include "Snake.hpp"
#include <utility>
#include "Snake.cpp"

using namespace std;

//i will add display method later 
// Helper to print the current snake coordinates
void printSnake(const Snake& s) {
    cout << "Snake Body: ";
    for (const auto& segment : s.getBody()) {
        cout << "(" << segment.first << "," << segment.second << ") ";
    }
    cout << endl;
}

int main() {
    // 1. Initialize Snake at (5, 5)
    Snake mySnake(5, 5);
    cout << "--- Initial State ---" << endl;
    printSnake(mySnake);

    // 2. Test Movement (Default is RIGHT)
    cout << "\n--- Moving Right 2 times ---" << endl;
    mySnake.move();
    mySnake.move();
    printSnake(mySnake);

    // 3. Test Growing
    cout << "\n--- Eating Food (Grow) and Moving ---" << endl;
    mySnake.grow();
    mySnake.move();
    printSnake(mySnake); // Should be length 2 now

    // 4. Test Direction Logic (Try to reverse - should be blocked)
    cout << "\n--- Attempting to reverse (Right to Left) ---" << endl;
    mySnake.setDirection(Direction::LEFT);
    mySnake.move();
    printSnake(mySnake); // Should still be moving Right or stay same, not reverse

    // 5. Test Self-Collision
    cout << "\n--- Testing Self-Collision ---" << endl;
    mySnake.grow(); mySnake.move();
    mySnake.grow(); mySnake.move();
    mySnake.grow(); mySnake.move();

    // Manual loop-back to hit itself
    mySnake.setDirection(Direction::UP);    mySnake.move();
    mySnake.setDirection(Direction::LEFT);  mySnake.move();
    mySnake.setDirection(Direction::DOWN);  mySnake.move();

    printSnake(mySnake);
    if (mySnake.checkSelfCollision()) {
        cout << "RESULT: Collision Detected! Game Over." << endl;
    }
    else {
        cout << "RESULT: No collision." << endl;
    }

    return 0;
}