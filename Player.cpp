#include "Player.h"
#include "KeyManager.h"
#include "Game.h"


// Handles a key press for the player.
// Checks if the pressed key matches one of the player's control keys.
// If a match is found, updates the player's movement direction accordingly.
void Player::handleKeyPress(char key_pressed) {
	if (carriedBy != nullptr) {
		carriedBy->handleKeyPress(key_pressed);
		return;
	}
	size_t index = 0;
	for (char k : keys) {
		if (std::tolower(k) == std::tolower(key_pressed)) {
			Direction requestedDir = (Direction)index;

			// Dealing with go throw each other and miss each other during launch
			if (isLaunched && game->isJustStartedCarry()) {

				if ((isHorizontal(launchDir) && isHorizontal(requestedDir)) ||
					(isVertical(launchDir) && isVertical(requestedDir))) {
					return;
				}
			}

			if (isOnSpring) {// Check if need to start launching
				if (requestedDir == Direction::STAY) {
					releaseSpring();// Start launching
					return;
				}

				if (currentSpring != nullptr && requestedDir != currentSpring->getEnterDir()) {
					releaseSpring();// Start launching
					return;
				}
			}

			if (isLaunched) {// During launch – only certain direction changes allowed

				// If his direction is horizontal and requested is vertical – allow change
				if (isHorizontal(launchDir) && isVertical(requestedDir)) {
					launchDir = requestedDir;
					return;
				}

				// If his direction is vertical and requested is horizontal – allow change
				if (isVertical(launchDir) && isHorizontal(requestedDir)) {
					launchDir = requestedDir;
					return;
				}

				// Otherwise, ignore the input
				return;
			}

			playerPoint.setDirection((Direction)index);
			return;
		}
		++index;
	}
}



// Resets the player for a new game.
void Player::resetForNewGame(const Point& p) {
	playerPoint = p;
	lives = LIVES;
	score = 0;
	disappeared = false;


	hasItem = false;
	removeItem();
	item = ItemType::NONE_ITEM;
	keyPassword = -1;
	originalKeyPos = Point(-1, -1, 0, 0, ' ');


	hasPendingRiddle = false;
	lastRiddlePos = Point(-1, -1, 0, 0, ' ');


	isOnSpring = false;
	currentSpring = nullptr;
	isLaunched = false;
	launchSpeed = 0;
	launchTimeLeft = 0;
	launchDir = Direction::STAY;

	carriedBy = nullptr;
	carrying = nullptr;
}

// Resets the player's position when entering a room.
void Player::resetInRoom(const Point& startPos) {
	playerPoint = startPos;
	disappeared = false;
}

//	Returns true if the player has moved from the original position.
bool Player::hasMoved(const Point& playerOrig) const {
	return (playerOrig.getX() != playerPoint.getX() ||
		playerOrig.getY() != playerPoint.getY());
}

// Moves the player in the specified direction.
void Player::move(Direction dir) {
	playerPoint.setDirection(dir);
	playerPoint.move();
}

// Moves the player according to its Point logic. and check if he colected items or reached a door.
MoveResult Player::move() {
	MoveResult res;

	if (room == nullptr) {
		res.type = MoveResultType::Blocked;
		return res;
	}

	Point playerOrig = playerPoint;// Save original position.

	playerPoint.move();
	if (game->isPlayerAt(playerPoint, this)) {
		playerPoint = playerOrig;
		res.type = MoveResultType::OtherPlayer;
		return res;
	}

	char cell = room->getCell(playerPoint);// Get the cell at the new position.

	if (handleRiddle(cell, playerOrig)) {
		res.type = MoveResultType::Riddle;
		return res;
	}

	if (handleObstacle(cell, playerOrig)) {
		res.type = MoveResultType::Obstacle;
		return res;
	}

	handlePickups(cell, playerOrig);
	handleSpring(cell, playerOrig);
	handleSwitch(cell, playerOrig);

	if (handleWallCollision(playerOrig)) {
		res.type = MoveResultType::Blocked;
		return res;
	}

	MoveResult doorRes = handleDoor(cell, playerOrig);
	if (doorRes.type != MoveResultType::None)
		return doorRes;

	res.type = MoveResultType::None;
	return res;
}

// Moves the player one step in the specified direction.
MoveResult Player::moveOneStep(Direction dir) {
	playerPoint.setDirection(dir);
	return move();
}

// Handles obstacle interaction when the player moves.
bool Player::handleObstacle(char cell, const Point& playerOrig) {
	if (cell != Room::OBSTACLE_CHAR)
		return false;

	Direction dir = directionFromPoints(playerOrig, playerPoint);

	int force = 1;
	if (game != nullptr) {
		force = game->calcEffectiveForce(*this, dir);
	}
	else {
		force = getForce();
	}

	room->startPushObstacleAt(playerPoint, dir, force);

	playerPoint = playerOrig;

	return true;
}

// Handles riddle interaction when the player moves.
bool Player::handleRiddle(char cell, const Point& playerOrig) {
	if (cell == Riddle::RIDDLE_CHAR) {
		lastRiddlePos = playerPoint;
		playerPoint = playerOrig;
		return true;
	}
	return false;
}

// Handles item pickups when the player moves.
void Player::handlePickups(char cell, const Point& playerOrig) {

	// Check for item pickups
	if (Bomb::isBombChar(cell) && !hasItem) {
		giveBomb();
		room->clearCell(playerPoint);
		room->printMessage("You got a bomb!");
	}

	if (cell == Room::TORCH_CHAR && !hasItem) {
		if (hasMoved(playerOrig)) {
			giveTorch();
			room->clearCell(playerPoint);
		}
		room->printMessage("You got a torch!");
	}

	if (KeyManager::isKeyChar(cell) && !hasItem) {
		if (hasMoved(playerOrig)) {
			Key* k = room->getKeyManager().getKeyAt(playerPoint);
			if (k) {
				giveKey(k->getPass());
				room->printMessage("You got a key!");
				room->clearCell(playerPoint);
			}
		}
	}
}

// Handles switch interaction when the player moves.
void Player::handleSwitch(char cell, const Point& playerOrig) {
	if (Switch::isSwitchChar(cell)) {
		if (hasMoved(playerOrig)) {
			room->changeSwitchAt(playerPoint);
			room->printMessage("You flipped the switch!");
		}

	}
}

// Handles wall collision when the player moves.
bool Player::handleWallCollision(const Point& playerOrig) {
	if (room->isWall(playerPoint)) {
		if (isOnSpring) {
			releaseSpring();
		}
		playerPoint = playerOrig;
		return true;
	}
	return false;
}

// Handles door interaction when the player moves.
MoveResult Player::handleDoor(char cell, const Point& playerOrig) {
	MoveResult out;

	if (room->isDoor(playerPoint)) { // Reached a door
		int doorNum = cell - '0';
		Point doorPos = playerPoint;

		int res = room->tryOpenDoor(doorNum, keyPassword);
		if (res > 0) { // Door opened
			if (res == 2) { // Opened with a key
				removeItem();
				keyPassword = -1;
			}

			playerPoint = playerOrig;
			setDisappeared(true); // Mark player as disappeared (moving to another room).

			Door* d = room->getDoorManager().getDoorAt(doorPos);
			if (d == nullptr) {
				out.type = MoveResultType::Blocked;
				return out;
			}

			out.type = MoveResultType::Door;
			out.roomId = d->getTargetRoom(); // Return the target room number.
			return out;
		}
		else {
			// Could not opening the door, either no key or wrong key.
			playerPoint = playerOrig;
			out.type = MoveResultType::Blocked;
			return out;
		}
	}

	out.type = MoveResultType::None;
	return out;
}


// Handles spring interaction when the player moves.
void Player::handleSpring(char cell, const Point& playerOrig) {
	if (Spring::isSpringChar(cell)) {
		if (!isOnSpring) {// First time on spring.
			Spring* s = room->getSpringAt(playerPoint);// Get the spring at the player's position.

			Direction playerEnterDir = directionFromPoints(playerOrig, playerPoint);

			if (!s || playerEnterDir != s->getEnterDir()) {// No spring found at this position. or wrong direction to enter spring.		
				playerPoint = playerOrig;
				return;
			}

			onSpring(*s);
			room->printMessage("You are on a spring!");
		}
		launchSpeed++;
		launchTimeLeft = launchSpeed * launchSpeed;
		room->clearCell(playerPoint);

	}
}

// Called when the player steps onto a spring.
void Player::releaseSpring() {
	if (!isOnSpring)
		return;

	launchDir = currentSpring->getExitDir();
	if (launchSpeed == 0)
		return;

	isLaunched = true;

	isOnSpring = false;
	room->printMessage("Start release Spring");
}

// Handles the player's launch movement.
void Player::handleLaunch() {
	if (!isLaunched)
		return;

	Point prev = playerPoint;
	bool shouldDrawDark = room->getIsDarkRoom() && !game->anyPlayerHasTorch();

	for (int i = 0; i < launchSpeed; ++i) {
		MoveResult res = moveOneStep(launchDir);
		game->checkPlayerMeeting(this, res);// Check for player meeting after each step.

		Point prevCarrierPos;
		if (carrying != nullptr) {
			carrying->followLeader(*this);
		}

		Point newPos = playerPoint;
		room->clearCell(prev);

		// Hit a wall or cannot move further
		if (res.type == MoveResultType::Blocked || res.type == MoveResultType::Obstacle) {
			room->drawSpring(shouldDrawDark);
			stopLaunch();
			return;
		}
	}

	launchTimeLeft--;
	if (launchTimeLeft <= 0) {
		room->drawSpring(shouldDrawDark);
		stopLaunch();
		return;
	}
}

// Stops the player's launch.
void Player::stopLaunch() {
	isLaunched = false;

	launchSpeed = 0;
	launchTimeLeft = 0;

	playerPoint.setDirection(launchDir);
	launchDir = Direction::STAY;

	if (carrying != nullptr) {
		carrying->carriedBy = nullptr;
		carrying = nullptr;
	}

}

// Follows the leader player's position.
void Player::followLeader(const Player& leader) {
	if (playerPoint.getX() == leader.playerPoint.getX() &&
		playerPoint.getY() == leader.playerPoint.getY())// Same position
		return;

	room->clearCell(playerPoint);

	playerPoint.setX(leader.playerPoint.getX());
	playerPoint.setY(leader.playerPoint.getY());
	playerPoint.setDirection(leader.launchDir);
	isOnSpring = false;
	currentSpring = nullptr;

}

// Adjusts the player's score by the specified delta.
void Player::addScore(int delta) {
	score += delta;
	if (score < 0) score = 0;
}




