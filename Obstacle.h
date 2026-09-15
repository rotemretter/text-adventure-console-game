#pragma once
#include <vector>
#include "Point.h"
#include "Direction.h"

class Room; // forward declaration

class Obstacle {
public:
    static constexpr char CH = '*';

private:
    std::vector<Point> cells;                // Cells occupied by the obstacle.
    bool moving = false;                     // Is the obstacle currently moving.
    Direction movingDir = Direction::STAY;	  // Direction of movement.
    bool pushedThisTick = false;			  // Was the obstacle pushed this tick.
    int stepsLeft = 0;					  // Steps left to move in the current push.

public:
    Obstacle() = default;
    explicit Obstacle(const std::vector<Point>& c) : cells(c) {}

    int size() const { return (int)cells.size(); }
    bool contains(const Point& p) const;

    bool isMoving() const { return moving; }
    Direction getDir() const { return movingDir; }

    bool startPush(Room& room, Direction dir, int appliedForce);

    void update(Room& room);

    void markPushed() { pushedThisTick = true; }
    void resetPushedFlag() { pushedThisTick = false; }

private:
    bool canShift(const Room& room, Direction dir) const;      // Check if the obstacle can shift in the given direction.
    void shift(Room& room, Direction dir);                     // Shift the obstacle in the given direction.
    bool containsXY(int x, int y) const;                       // Check if the obstacle contains a cell at (x, y).
};








