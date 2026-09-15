#pragma once
#include "Point.h"

class Bomb {
private:
	int timer; 		          // countdown timer.
	bool isActive;	         // is the bomb active
	Point location;         // location of the bomb
	const int radius = 3;  // explosion radius
	char owner = '\0';    // owner of the bomb //TODO: add to constructor

public:
	static constexpr char BOMB_CHAR = '@';

	// Default constructor
	Bomb()
		:timer(5),
		isActive(true),
		location(0, 0, 0, 0, '@') {
	};

	Bomb(const Point& p)
		: timer(5),
		isActive(true),
		location(p),
		radius(3) {
	}

	Bomb(const Point& p, int timer, char owner)
		: timer(timer),
		isActive(true),
		location(p),
		owner(owner) {
	}

	void placeAt(const Point& p); // the place where the bomb is set
	void activate();              //start the countdown and isActive = true
	void decreaseTimer();         // decrease timer by 1
	bool shouldExplode() const;   // return true if timer reached 0
	void deactivate();            // after explosion set isActive = false

	static bool isBombChar(char c) { return c == BOMB_CHAR; }
	const Point& getLocation() const { return location; }
	int getRadius() const { return radius; }
	bool isActiveNow() const { return isActive; }

	void setOwner(char c) { owner = c; }
	char getOwner() const { return owner; }
};









