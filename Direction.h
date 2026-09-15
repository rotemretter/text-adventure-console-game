#pragma once

class Point;

enum class Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	STAY
};

// Determines the direction from one point to another.
Direction directionFromPoints(const Point& from, const Point& to);

int dirToDx(Direction d);
int dirToDy(Direction d);
void updatePositionByDir(char dirChar, int index, int& x, int& y);
Direction charToDirection(char c);
char getOppositeChar(char dirChar);

// True if direction is LEFT/RIGHT.
inline bool isHorizontal(Direction d) {
	return d == Direction::LEFT || d == Direction::RIGHT;
}

// True if direction is UP/DOWN.
inline bool isVertical(Direction d) {
	return d == Direction::UP || d == Direction::DOWN;
}









