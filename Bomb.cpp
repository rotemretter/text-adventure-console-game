#include "Bomb.h"

// Bomb class methods implementation.
//-----------------------------------------------//

// Activates the bomb by setting the timer and marking it as active.
void Bomb::activate() {
	timer = 5;
	isActive = true;
}

// Decreases the bomb's timer by 1 if it is active and the timer is greater than 0.
void Bomb::decreaseTimer() {
	if (isActive && timer > 0) {
		timer--;
	}
}

// Checks if the bomb should explode (timer has reached 0).
bool Bomb::shouldExplode() const {
	if (timer == 0) {
		return true;
	}
	return false;
}

// Deactivates the bomb by marking it as inactive.
void Bomb::deactivate() {
	isActive = false;
}

// Places the bomb at a specific location and resets its state.
void Bomb::placeAt(const Point& p) {
	location = p;
	isActive = false;
	timer = 0;
}












