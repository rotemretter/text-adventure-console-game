#include "Obstacle.h"
#include "Room.h"  

// Obstacle character representation
bool Obstacle::contains(const Point& p) const {
    return containsXY(p.getX(), p.getY());
}

// Check if the obstacle contains a cell at (x, y)
bool Obstacle::containsXY(int x, int y) const {
    for (const Point& c : cells) {
        if (c.getX() == x && c.getY() == y) return true;
    }
    return false;
}

// Check if the obstacle can shift in the given direction within the room
bool Obstacle::canShift(const Room& room, Direction dir) const {
    int dx = dirToDx(dir);
    int dy = dirToDy(dir);

    for (const Point& c : cells) {
        int nx = c.getX() + dx;
        int ny = c.getY() + dy;

        if (room.isOutsideGameArea(nx, ny))
            return false;

        char dest = room.getCell(Point(nx, ny));

        if (containsXY(nx, ny))
            continue;

        if (dest != ' ') return false;
    }
    return true;
}

// Shift the obstacle in the given direction within the room
void Obstacle::shift(Room& room, Direction dir) {
    int dx = dirToDx(dir);
    int dy = dirToDy(dir);

    for (const Point& c : cells) {
        room.setCellChar(Point(c.getX(), c.getY()), ' ');
    }

    for (Point& c : cells) {
        c = Point(c.getX() + dx, c.getY() + dy, 0, 0, CH);
    }

    for (const Point& c : cells) {
        room.setCellChar(Point(c.getX(), c.getY()), CH);
    }
}

// Start pushing the obstacle in the given direction with the applied force
bool Obstacle::startPush(Room& room, Direction dir, int appliedForce) {
    if (appliedForce < size())
        return false;

    if (!canShift(room, dir))
        return false;

    moving = true;
    movingDir = dir;
    pushedThisTick = true;
    stepsLeft = appliedForce;
    return true;
}

// Update the obstacle's position in the room
void Obstacle::update(Room& room) {
    if (!moving) return;

    if (!pushedThisTick) {
        moving = false;
        movingDir = Direction::STAY;
        return;
    }

    pushedThisTick = false;

    if (canShift(room, movingDir)) {
        for (int i = 0; i < stepsLeft; i++) {
            shift(room, movingDir);

        }
        stepsLeft = 0;
    }
    else {
        moving = false;
        movingDir = Direction::STAY;
    }
}








