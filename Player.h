
#pragma once
#include "Point.h"
#include "Room.h"
#include "Riddle.h"
#include "Spring.h"
#include "Direction.h"
#include "MoveResult.h"

enum ItemType { NONE_ITEM, KEY, BOMB, TORCH };// The object types the player can hold.
class Game;

class Player {// Represents a player controlled by keyboard input.

private:
	Game* game = nullptr;          // Back-pointer to Game (set by Game).
	int lives = LIVES;             // Current lives.
	static const int SIZE = 5;     // Number of control keys.
	Point playerPoint;             // Current position + symbol + direction.
	char keys[SIZE];               // Key mappings for movement/actions.

	Room* room;                    // Current room pointer.

	bool hasItem;                  // Holding an item?
	ItemType item;                 // Held item type.
	Point originalKeyPos;          // Key original position (if needed to restore).
	bool disappeared;              // True when player exited through a door.
	int keyPassword = -1;          // Key password/id.
	Point lastRiddlePos;           // Last riddle cell position.
	bool hasPendingRiddle = false; // Pending riddle request flag.

	bool isOnSpring = false;               // Standing on a spring?
	const Spring* currentSpring = nullptr; // Active spring pointer.
	bool isLaunched = false;               // Currently being launched?
	Direction launchDir;                   // Launch direction.
	int launchSpeed;                       // Launch speed/force.
	int launchTimeLeft;                    // Launch ticks remaining.
	int score = 0;                         // Player score.

	Player* carriedBy = nullptr;           // If carried: pointer to carrier.
	Player* carrying = nullptr;            // If carrying: pointer to carried player.

	int id;                             // Player ID (1 or 2).


public:
	static constexpr int LIVES = 3;


	// Default constructor
	Player()
		: playerPoint(),
		room(nullptr),
		hasItem(false),
		item(NONE_ITEM),
		originalKeyPos(-1, -1),
		disappeared(false),
		currentSpring(),
		isLaunched(false),
		launchDir(Direction::STAY),
		launchSpeed(0),
		launchTimeLeft(0)
	{
		memset(keys, 0, SIZE * sizeof(keys[0]));
	}

	// Constructs a player with a starting point and a set of control keys.
	Player(const Point& point, const char(&the_keys)[SIZE + 1], Room* theRoom)
		: playerPoint(point),
		room(theRoom),
		hasItem(false),
		item(NONE_ITEM),
		disappeared(false),
		currentSpring(),
		isLaunched(false),
		launchDir(Direction::STAY),
		launchSpeed(0),
		launchTimeLeft(0)
	{
		memcpy(keys, the_keys, SIZE * sizeof(keys[0]));
	}

	void move(Direction dir);
	MoveResult move();
	MoveResult moveOneStep(Direction dir);

	int getId() const { return id; }
	void setId(int newId) { id = newId; }

	// Sets the owning Game pointer (used for callbacks/coordination).
	void setGame(Game* g) {
		game = g;
	}
	int getLives() const { return lives; }
	bool isAlive() const { return lives > 0; }

	void addScore(int delta);
	int getScore() const { return score; }

	// Decrements lives by 1 (if any left).
	void loseLife() {
		if (lives > 0)
			lives--;
	}

	// Resets lives to default value.
	void resetLives() {
		lives = LIVES;
	}

	// Returns current player position.
	const Point& getPoint() const {
		return playerPoint;
	}

	// Returns whether the player exited through a door and should not be drawn.
	bool hasDisappeared() const {
		return disappeared;
	}

	// Sets disappeared flag (default: true).
	void setDisappeared(bool v = true) {
		disappeared = v;
	}

	// Sets the current room pointer.
	void setRoom(Room* newRoom) {
		room = newRoom;
	}

	// Returns current room pointer.
	Room* getRoom() const {
		return room;
	}
	void resetForNewGame(const Point& startPos);
	void resetInRoom(const Point& startPos);

	// Draws player using a provided character.
	void draw(char c) {
		playerPoint.draw(c);
	}

	// Draws player using the Point's stored character.
	void draw() {
		playerPoint.draw();
	}

	// Items functions:
	//-----------------------------------------------//
	// Check if the player has any item.
	bool hasAnyItem() const {
		return hasItem && item != NONE_ITEM;
	}

	// Returns the current held item type.
	ItemType getItemType() const {
		return item;
	}


	// Get the type of item the player is holding now.
	ItemType getHeldItem() const {
		return hasItem ? item : NONE_ITEM;
	}

	// Clears held item state.
	void removeItem() {
		hasItem = false;
		item = NONE_ITEM;
	}

	//*****************************************************************************
	// Check if the player has moved from the original position.
	bool hasMoved(const Point& playerOrig) const;

	// Handle functions:
	//-----------------------------------------------//
	void handleKeyPress(char key);
	void handlePickups(char cell, const Point& playerOrig);
	void handleSwitch(char cell, const Point& playerOrig);
	bool handleWallCollision(const Point& playerOrig);
	MoveResult handleDoor(char cell, const Point& playerOrig);
	bool handleRiddle(char cell, const Point& playerOrig);
	void handleSpring(char cell, const Point& playerOrig);
	bool handleObstacle(char cell, const Point& playerOrig);

	// KEYS function:
	//-----------------------------------------------//
	// Check if the player has a key item.
	bool hasKeyItem() const {
		return hasItem && item == KEY;
	}

	// The player picks up a key.
	void giveKey(int pass) {
		hasItem = true;
		item = KEY;
		keyPassword = pass;
	}

	// Get the password associated with the key item.
	int getKeyPass() const {
		return keyPassword;
	}

	//BOMBS function:
	//-----------------------------------------------//
	bool hasBombItem() const {
		return hasItem && item == BOMB;
	}

	// the player picks up a bomb
	void giveBomb() {
		hasItem = true;
		item = BOMB;
	}


	//TORCHES function:
	//-----------------------------------------------//
	bool hasTorchItem() const {
		return hasItem && item == TORCH;
	}

	// the player picks up a torch
	void giveTorch() {
		hasItem = true;
		item = TORCH;
	}

	//RIDDLE function:
	//-----------------------------------------------//
	bool hasRiddleRequest() const { return hasPendingRiddle; }
	Point getLastRiddlePos() const { return lastRiddlePos; }
	void clearRiddleRequest() { hasPendingRiddle = false; }
	void setPoint(const Point& p) { playerPoint = p; }
	void resetDirection() { playerPoint.setDirection(Direction::STAY); }

	//SPRINGS function:
	//-----------------------------------------------//
	void releaseSpring();
	void handleLaunch();
	void stopLaunch();
	bool getIsOnSpring() const { return isOnSpring; }
	Spring* getActiveSpring() const { return (Spring*)currentSpring; }
	void followLeader(const Player& leader);

	// Returns the current launch speed (0 if not launched).
	int getLaunchSpeed() const {
		if (!isCurrentlyLaunched())
			return 0;

		return launchSpeed;
	}

	// Returns whether the player is currently launched.
	bool isCurrentlyLaunched() const {
		return isLaunched;
	}

	// Sets the launched state to true.
	void SetisLunched() {
		isLaunched = true;
	}

	// Handles the event of stepping on a spring.
	void onSpring(const Spring& s) {
		isOnSpring = true;
		currentSpring = &s;
		launchDir = s.getExitDir();
	}

	//CARRY function:
	//-----------------------------------------------//
	// Sets the carriedBy pointer and updates the carrier's carrying pointer.
	void setCarriedBy(Player* leader) {
		carriedBy = leader;
		if (leader != nullptr) {
			leader->carrying = this;
		}
	}

	// Clears carry links for both carrier and carried players.
	void clearCarryLinks() {
		if (carriedBy != nullptr) {
			carriedBy->carrying = nullptr;
		}
		if (carrying != nullptr) {
			carrying->carriedBy = nullptr;
		}

		carriedBy = nullptr;
		carrying = nullptr;
	}

	// Checks if the player is currently being carried.
	bool isCarried() const {
		return carriedBy != nullptr;
	}

	// Returns the character symbol representing the player.
	char getSymbol() const {
		return playerPoint.getChar();
	}


	//OBSTACLE function:
	//-----------------------------------------------//
	// Returns the current movement direction.
	Direction getMoveDir() const {
		if (isCurrentlyLaunched())
			return launchDir;
		return playerPoint.getDirection();
	}

	// Returns whether the player is trying to move.
	bool isTryingToMove() const {
		if (isCurrentlyLaunched())
			return launchSpeed > 0 && launchDir != Direction::STAY;
		return playerPoint.getDirection() != Direction::STAY;
	}

	// Returns pushing force (launch speed when launched, otherwise 1).
	int getForce() const {
		if (!isCurrentlyLaunched())
			return 1;
		return launchSpeed;
	}

};




