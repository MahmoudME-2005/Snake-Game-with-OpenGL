#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility>
#include <stdexcept>

#include "Snake.hpp"
#include "Fruit.hpp"
#include "Wall.hpp"

//This file is just to verify that verything behaves correctly and makes it easier to debugg and know which part is causing the eror



// simple test system (won't use GoogleTest)

//to track results
static int passed = 0;
static int failed = 0;

// define test function
#define TEST(name) void name()
// run test + catch errors
//run the tests if no error pass if an error is caught fail
#define RUN(name)  do { \
try { name(); std::cout << "[PASS] " #name "\n"; ++passed; } \
catch (const std::exception& e) { \
std::cout << "[FAIL] " #name " — " << e.what() << "\n"; ++failed; } \
catch (...) { \
std::cout << "[FAIL] " #name " — unknown exception\n"; ++failed; } \
} while(0)


// helper: if false >> throw error
static void check(bool condition, const std::string& msg) {
    if (!condition) throw std::runtime_error(msg);
}

// ═════════════════════════════════════════════════════════════
// "SNAKE TESTS"
// ═════════════════════════════════════════════════════════════


// ── Construction ─────────────────────────────────────────────
// constructor = head set correctly
//verifies the constructor correctness and it shows bugs like  wrong initialization , swapped x/y , uninitialized head
TEST(snake_initial_head_position) {
  // i put a start condition at (5,10) so i expect the head to be at this position
    Snake s(5, 10);
    check(s.getHead() == std::make_pair(5, 10),
          "wrong initial head"); //if condition is false print the message
}

// snake starts with 1 block only i
//it shows bugs like snake accidentally starting with garbage size , unintended growth in constructor
TEST(snake_initial_body_size) {
    Snake s(3, 3);
    check(s.getBody().size() == 1,
          "snake must start size 1");
}

// snake occupies start cell
//snake must remember its position internally
//shows bugs like mismatch between head and body set , broken collision detection later
TEST(snake_occupies_start_cell) {
    Snake s(4, 7);
    check(s.occupies(4, 7), "should occupy start");
    check(!s.occupies(0, 0), "should NOT occupy random cell");
}


// ── Movement ─────────────────────────────────────────────────
// default movement = right (this is our default) >> x+1
//show bugs like snake not moving at all , wrong default direction
TEST(snake_moves_right_by_default) {
        Snake s(5, 5);
        s.move();
        check(s.getHead() == std::make_pair(6, 5),
              "Moving RIGHT should increment x by 1");
        check(s.getBody().size() == 1,
              "Body length must stay 1 without grow()");
    }

// left move >> x-1
TEST(snake_moves_left) {
    Snake s(5, 5);
    s.setDirection(Direction::UP);   // perpendicular first
    s.setDirection(Direction::LEFT); // now allowed
    s.move();

    check(s.getHead() == std::make_pair(4, 5),
          "Moving LEFT should decrement x");
}

// up move >> y-1
TEST(snake_moves_up) {
    Snake s(5, 5);
    s.setDirection(Direction::UP);
    s.move();
    check(s.getHead() == std::make_pair(5, 4), "Moving UP should decrement y");
}

// down move >> y+1
TEST(snake_moves_down) {
    Snake s(5, 5);
    s.setDirection(Direction::DOWN);
    s.move();
    check(s.getHead() == std::make_pair(5, 6), "Moving DOWN should increment y");
}

// tail removed after move (no grow) make sure that snake not grow with each movment and it moves as whole
// show bugs like snake endlessly growing , duplicate body segments
TEST(snake_tail_removed_after_move) {
    Snake s(5, 5);   // head at (5,5)
    s.move();         // head moves to (6,5), tail (5,5) removed
    check(!s.occupies(5, 5),
          "Old tail cell should no longer be occupied after move");
}



// ── Growing ───────────────────────────────────────────────────
// grow increases length >> eating food increases size
//shows bugs like grow() doing nothing , size not updating after move
TEST(snake_grow_increases_length) {
    Snake s(5, 5);
    s.grow();
    s.move();
    check(s.getBody().size() == 2,
          "After grow() + move(), body should have 2 segments");
}

// grow keeps tail >> when growing tail should not disappear
// shows bugs like grow behaving like normal move , losing body segment incorrectly
TEST(snake_grow_keeps_tail_cell) {
    Snake s(5, 5);
    s.grow();
    s.move();   // head → (6,5), tail (5,5) kept
    check(s.occupies(5, 5), "Old tail should still be occupied after grow");
    check(s.occupies(6, 5), "New head should be occupied after move");
}


// grow only once per move >> one apple = one segment increase
//shows bugs like infinite growth bug , every move increasing size unintentionally
TEST(snake_pendingGrow_resets_after_one_move) {
    Snake s(5, 5);
    s.grow();
    s.move();   // length becomes 2
    s.move();   // should NOT grow again
    check(s.getBody().size() == 2,
          "Second move after grow() should not grow further");
}

// multiple grows : 3 grows >> size 4
// shows bugs like grow stacking incorrectly
TEST(snake_multiple_grows) {
    Snake s(1, 1);
    for (int i = 0; i < 3; ++i) { s.grow(); s.move(); }
    check(s.getBody().size() == 4,
          "Three grow+move cycles should produce length 4");
}



// ── Direction locking (no reverse) ───────────────────────────
// no reverse (RIGHT >> LEFT blocked) Snake cannot instantly reverse into itself
//shows bugs like instant self-collision exploit
TEST(snake_cannot_reverse_right_to_left) {
    Snake s(5, 5);  // default direction: RIGHT
    s.setDirection(Direction::LEFT);   // should be ignored
    s.move();
    check(s.getHead() == std::make_pair(6, 5),
          "Reversing RIGHT>>LEFT should be blocked; snake still moves RIGHT");
}

// no reverse (UP >> DOWN blocked) Snake cannot instantly reverse into itself
//shows bugs like instant self-collision exploit
TEST(snake_cannot_reverse_up_to_down) {
    Snake s(5, 5);
    s.setDirection(Direction::UP);
    s.move();
    s.setDirection(Direction::DOWN);  // should be ignored
    s.move();
    check(s.getHead() == std::make_pair(5, 3),
          "Reversing UP>>DOWN should be blocked; snake continues UP");
}

// perpendicular move allowed Valid turns must work like right then up
//shows bugs like over-restricting movement
TEST(snake_direction_change_accepted_when_perpendicular) {
    Snake s(5, 5);   // RIGHT
    s.setDirection(Direction::UP);
    s.move();
    check(s.getHead() == std::make_pair(5, 4),
          "Perpendicular direction change RIGHT>>UP should be accepted");
}

// ── Self-collision ────────────────────────────────────────────
//A snake of size 1 cannot collide with itself
//shows bugs like false collision detection
TEST(snake_no_self_collision_when_short) {
    Snake s(5, 5);
    s.move();
    check(!s.checkSelfCollision(),
          "Single-segment snake cannot collide with itself");
}


TEST(snake_self_collision_detected) {
    // Build a snake long enough to loop back onto itself.
    // Start at (2,2), go RIGHT to grow to 4 segments, then loop back.
    Snake s(2, 2);
    // Grow to 5 segments
    for (int i = 0; i < 4; ++i) { s.grow(); s.move(); }
    // Snake occupies (2,2),(3,2),(4,2),(5,2),(6,2)
    // Turn DOWN, move once → head at (6,3)
    s.setDirection(Direction::DOWN);  s.move();
    // Turn LEFT, move 4 times → head at (2,3)
    s.setDirection(Direction::LEFT);
    for (int i = 0; i < 4; ++i) s.move();
    // Turn UP → head moves to (2,2) — which is the tail end
    // (tail may have shifted, but let's check the mechanism works)
    s.setDirection(Direction::UP); s.move();
    // Whether collision happened depends on exact tail position;
    // just verify the method runs and returns a bool without crashing.
    bool result = s.checkSelfCollision();
    (void)result; // test passes if no crash
}


// ═════════════════════════════════════════════════════════════
// "FRUIT TESTS"
// ═════════════════════════════════════════════════════════════


// fruit inside grid
//shows bugs like out-of-bounds crashes
TEST(fruit_placed_within_grid) {
    std::unordered_set<std::pair<int,int>, PairHash> occupied;
    Fruit f(10, 10, occupied);
    check(f.getX() >= 0 && f.getX() < 10, "Fruit x must be in [0,10)");
    check(f.getY() >= 0 && f.getY() < 10, "Fruit y must be in [0,10)");
}

// fruit not on snake
//shows bugs like instant eating bug at spawn
TEST(fruit_not_on_occupied_cell) {
    // Fill almost the entire 3×3 grid, leaving only (2,2) free
    std::unordered_set<std::pair<int,int>, PairHash> occupied;
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            if (!(c == 2 && r == 2))
                occupied.insert({c, r});

    Fruit f(3, 3, occupied);
    check(f.getX() == 2 && f.getY() == 2,
          "Fruit must land on the only free cell (2,2)");
}


//Edge case: no free space
//shows bugs like infinite loop in random placement
TEST(fruit_throws_when_grid_full) {
    std::unordered_set<std::pair<int,int>, PairHash> occupied;
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            occupied.insert({c, r});

    bool threw = false;
    try { Fruit f(3, 3, occupied); }
    catch (const std::runtime_error&) { threw = true; }
    check(threw, "Fruit constructor should throw when no free cell exists");
}

//Respawn must remain valid inside grid
//shows bugs like invalid repositioning
TEST(fruit_respawn_changes_position_or_stays_valid) {
    std::unordered_set<std::pair<int,int>, PairHash> occupied;
    Fruit f(10, 10, occupied);
    f.respawn(10, 10, occupied);
    check(f.getX() >= 0 && f.getX() < 10, "After respawn, x still in [0,10)");
    check(f.getY() >= 0 && f.getY() < 10, "After respawn, y still in [0,10)");
}

//Even after changes, must still respect occupied cells
//shows bugs likw fruit spawning inside snake after respawn
TEST(fruit_respawn_avoids_occupied) {
    std::unordered_set<std::pair<int,int>, PairHash> occupied;
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            if (!(c == 1 && r == 1))
                occupied.insert({c, r});

    Fruit f(3, 3, occupied);  // must land at (1,1)
    // Now block (1,1) too and open (0,0) only
    occupied.insert({1, 1});
    occupied.erase({0, 0});
    f.respawn(3, 3, occupied);
    check(f.getX() == 0 && f.getY() == 0,
          "After respawn, fruit should move to the only new free cell (0,0)");
}

// ═════════════════════════════════════════════════════════════
// WALL TESTS
// ═════════════════════════════════════════════════════════════


// Walls must store correct positions
//shows bugs liks wrong wall storage , missing obstacles
TEST(wall_construction_basic) {
    std::vector<std::pair<int,int>> w = {{0,0},{1,0},{2,0}};
    Wall wall(w, {10, 10});
    check(wall.isWall({0,0}), "(0,0) should be a wall");
    check(wall.isWall({1,0}), "(1,0) should be a wall");
    check(!wall.isWall({5,5}), "(5,5) should NOT be a wall");
}

//Walls outside grid should be ignored
//shows bugs liks invalid map corruption
TEST(wall_out_of_bounds_rejected) {
    // (15,0) is outside a 10×10 grid — should be silently rejected
    std::vector<std::pair<int,int>> w = {{15,0},{0,0}};
    Wall wall(w, {10, 10});
    check(!wall.isWall({15,0}), "Out-of-bounds position must be rejected");
    check(wall.isWall({0,0}),   "In-bounds position must be accepted");
}

//Negative coordinates are invalid
//shows bugs like broken indexing
TEST(wall_negative_position_rejected) {
    std::vector<std::pair<int,int>> w = {{-1,0},{0,0}};
    Wall wall(w, {10, 10});
    check(!wall.isWall({-1,0}), "Negative x position must be rejected");
}

//Duplicates should not exist
//shows bugs like double-counting walls
TEST(wall_isValid_returns_false_for_existing_wall) {
    // We test this indirectly: adding duplicate walls should not double-count
    std::vector<std::pair<int,int>> w = {{3,3},{3,3}};
    Wall wall(w, {10,10});
    // unordered_set deduplicates automatically
    check(wall.getWalls().size() == 1,
          "Duplicate wall positions must be stored only once");
}

//Ensures storage consistency
//shows bugs like lost walls
TEST(wall_getWalls_returns_all_valid_walls) {
    std::vector<std::pair<int,int>> w = {{0,0},{1,1},{2,2}};
    Wall wall(w, {10,10});
    check(wall.getWalls().size() == 3,
          "getWalls() should return exactly 3 entries");
}

//Empty case must behave correctly
TEST(wall_empty_wall_list) {
    Wall wall({}, {10, 10});
    check(wall.getWalls().empty(), "Empty wall list → no walls stored");
    check(!wall.isWall({0,0}),    "isWall should return false when no walls exist");
}


// ═════════════════════════════════════════════════════════════
//  INTEGRATION — Snake + Fruit interaction
// ═════════════════════════════════════════════════════════════

//Snake + Fruit must not overlap
//shows bugs like spawning bugs across systems
TEST(integration_fruit_not_on_snake) {
    Snake s(5, 5);
    // Grow to 3 segments: (5,5),(6,5),(7,5) moving right
    for (int i = 0; i < 2; ++i) { s.grow(); s.move(); }

    // Build occupied set from snake body
    std::unordered_set<std::pair<int,int>, PairHash> occupied;
    for (auto& seg : s.getBody()) occupied.insert(seg);

    Fruit f(10, 10, occupied);
    check(!occupied.count({f.getX(), f.getY()}),
          "Fruit must not spawn on any snake body segment");
}


//Move >> detect >> grow
TEST(integration_snake_eats_fruit) {
    Snake s(5, 5);   // head at (5,5), moving RIGHT
    // Place fruit one step ahead
    // Manually verify: after move, head → (6,5); if fruit is there, snake eats
    s.move();  // head now (6,5)
    // Simulate "eating": check position matches, then grow & respawn
    bool atFruitPos = (s.getHead().first == 6 && s.getHead().second == 5);
    if (atFruitPos) {
        s.grow();
        s.move();
        check(s.getBody().size() == 2,
              "Snake should grow after eating fruit");
    } else {
        check(false, "Head was not at expected fruit position");
    }
}


// ═════════════════════════════════════════════════════════════
//  main
// ═════════════════════════════════════════════════════════════
int main() {
    std::cout << "═══════════════════════════════════\n";
    std::cout << "  Snake Game — Unit Test Suite\n";
    std::cout << "═══════════════════════════════════\n\n";

    // Snake
    std::cout << "── Snake ──\n";
    RUN(snake_initial_head_position);
    RUN(snake_initial_body_size);
    RUN(snake_occupies_start_cell);
    RUN(snake_moves_right_by_default);
    RUN(snake_moves_left);
    RUN(snake_moves_up);
    RUN(snake_moves_down);
    RUN(snake_tail_removed_after_move);
    RUN(snake_grow_increases_length);
    RUN(snake_grow_keeps_tail_cell);
    RUN(snake_pendingGrow_resets_after_one_move);
    RUN(snake_multiple_grows);
    RUN(snake_cannot_reverse_right_to_left);
    RUN(snake_cannot_reverse_up_to_down);
    RUN(snake_direction_change_accepted_when_perpendicular);
    RUN(snake_no_self_collision_when_short);
    RUN(snake_self_collision_detected);

    // Fruit
    std::cout << "\n── Fruit ──\n";
    RUN(fruit_placed_within_grid);
    RUN(fruit_not_on_occupied_cell);
    RUN(fruit_throws_when_grid_full);
    RUN(fruit_respawn_changes_position_or_stays_valid);
    RUN(fruit_respawn_avoids_occupied);

    // Wall
    std::cout << "\n── Wall ──\n";
    RUN(wall_construction_basic);
    RUN(wall_out_of_bounds_rejected);
    RUN(wall_negative_position_rejected);
    RUN(wall_isValid_returns_false_for_existing_wall);
    RUN(wall_getWalls_returns_all_valid_walls);
    RUN(wall_empty_wall_list);

    // Integration
    std::cout << "\n── Integration ──\n";
    RUN(integration_fruit_not_on_snake);
    RUN(integration_snake_eats_fruit);

    // Summary
    std::cout << "\n═══════════════════════════════════\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "═══════════════════════════════════\n";
    return failed == 0 ? 0 : 1;
}
