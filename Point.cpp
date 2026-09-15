#include "Point.h" 
#include "utils.h" 

// Updates the position so that if it goes out of the screen boundaries, it wraps around to the opposite side.
void Point::move() {
	x = (x + diff_x + SCREEN_WIDTH) % SCREEN_WIDTH;
	y = (y + diff_y + SCREEN_HEIGHT) % SCREEN_HEIGHT;
}

// Sets the movement direction based on the provided Direction enum value.
void Point::setDirection(Direction dir) {
	switch (dir) {
	case Direction::UP:
		diff_x = 0;
		diff_y = -1;
		break;
	case Direction::RIGHT:
		diff_x = 1;
		diff_y = 0;
		break;
	case Direction::DOWN:
		diff_x = 0;
		diff_y = 1;
		break;
	case Direction::LEFT:
		diff_x = -1;
		diff_y = 0;
		break;
	case Direction::STAY:
		diff_x = 0;
		diff_y = 0;
		break;
	}
}


// Returns the current movement direction as a Direction enum value.
Direction Point::getDirection() const {
	if (diff_x == 1 && diff_y == 0) return Direction::RIGHT;
	if (diff_x == -1 && diff_y == 0) return Direction::LEFT;
	if (diff_x == 0 && diff_y == 1) return Direction::DOWN;
	if (diff_x == 0 && diff_y == -1) return Direction::UP;
	return Direction::STAY;
}














