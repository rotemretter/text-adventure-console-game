#pragma once
#include "Point.h"
#include "Direction.h"
#include <vector>

class Spring
{
private:
	std::vector<Point> points;     // The points occupied by the spring.
	Direction enterDir;           // The direction from which the player enters the spring.
	Direction exitDir;           // The direction in which the player exits the spring.

public:
	static constexpr char SPRING_CHAR = '#';

	Spring() = delete;

	Spring(const std::vector<Point>& pts,
		Direction enter,
		Direction exit)
		: points(pts),
		enterDir(enter),
		exitDir(exit) {
	}

	const std::vector<Point>& getPoints() const { return points; }
	static bool isSpringChar(char c) { return c == SPRING_CHAR; }

	Direction getEnterDir() const { return enterDir; }
	Direction getExitDir()  const { return exitDir; }

	const Point& getLocation() const {
		return points.front();
	}

};




