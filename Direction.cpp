#include "Direction.h"
#include "Point.h"

// Determines the direction from one point to another.
Direction directionFromPoints(const Point& from, const Point& to)
{
    if (to.getX() > from.getX()) return Direction::RIGHT;
    if (to.getX() < from.getX()) return Direction::LEFT;
    if (to.getY() > from.getY()) return Direction::DOWN;
    if (to.getY() < from.getY()) return Direction::UP;

    return Direction::STAY;
}

// Returns X delta for a Direction.
int dirToDx(Direction d)
{
    if (d == Direction::RIGHT) return 1;
    if (d == Direction::LEFT)  return -1;
    return 0;
}

// Returns Y delta for a Direction.
int dirToDy(Direction d)
{
    if (d == Direction::DOWN) return 1;
    if (d == Direction::UP)   return -1;
    return 0;
}

// Converts a direction character (U/D/L/R) to dx,dy deltas.
void getDelta(char dirChar, int& dx, int& dy) {
    dx = 0;
    dy = 0;
    switch (dirChar) {
    case 'U': dy = -1; break; // Up
    case 'D': dy = 1;  break; // Down
    case 'L': dx = -1; break; // Left
    case 'R': dx = 1;  break; // Right
    }
}

// Updates x,y by moving 'index' steps along the given direction character.
void updatePositionByDir(char dirChar, int index, int& x, int& y) {
    if (dirChar == 'D' || dirChar == 'U') {
        y += index;
    }
    else if (dirChar == 'R' || dirChar == 'L') {
        x += index;
    }
}

// Returns the opposite direction character (U<->D, L<->R).
char getOpposite(char dirChar) {
    switch (dirChar) {
    case 'U': return 'D';
    case 'D': return 'U';
    case 'L': return 'R';
    case 'R': return 'L';
    default:  return dirChar;
    }
}

// Converts a direction character (U/D/L/R) to the Direction enum.
Direction charToDirection(char c) {
    switch (c) {
    case 'U': return Direction::UP;
    case 'D': return Direction::DOWN;
    case 'L': return Direction::LEFT;
    case 'R': return Direction::RIGHT;
    default:  return Direction::STAY;
    }
}

// Returns the opposite direction character (duplicate of getOpposite).
char getOppositeChar(char dirChar) {
    switch (dirChar) {
    case 'U': return 'D';
    case 'D': return 'U';
    case 'L': return 'R';
    case 'R': return 'L';
    default:  return dirChar;
    }
}









