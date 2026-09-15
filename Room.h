#pragma once
#include "DoorManager.h"
#include "KeyManager.h"
#include "Point.h"
#include "utils.h"
#include "Bomb.h"
#include "Switch.h"
#include "Riddle.h"
#include "Spring.h"
#include "Obstacle.h"
#include "Direction.h"
#include "GameEvent.h"
#include <vector>
#include <string>
#include <filesystem>


class Room {

private:
	int id;                                        // Room number.
	char layout[SCREEN_HEIGHT][SCREEN_WIDTH + 1]; // Room sizes.
	bool isLoaded = false;		      			 // Is the room loaded successfully.
	DoorManager doorManager;                    // Door manager for this room.
	KeyManager keyManager;                     // Key manager for this room.
	std::vector<Bomb> bombs;		          // Active bombs in the room.
	std::vector<Switch> switches;		     // Switches in the room.
	bool isDarkRoom = false;		        // Is the room dark
	std::vector<Riddle> riddles;	       // Riddles in the room.
	std::vector<Spring> springs;	      // Springs in the room.
	std::vector<Obstacle> obstacles;     // Obstacles in the room. //TODO: add to constructor and loadRoom
	std::vector<GameEvent> eventsThisFrame;   // Events that occurred in the room this frame. //TODO: add to constructor and loadRoom

	bool hasP1 = false;                      // Player 1 start position found.
	bool hasP2 = false;				  	    // Player 2 start position found.
	Point p1Start;                         // Player 1 start position.
	Point p2Start;                        // Player 2 start position.

	Point legendPos;                    // Legend position.
	bool hasLegend = false;            // Does the room have a legend position.

public:
	// Constants for game area boundaries and message/timer display positions.
	static constexpr int MESSAGE_X = 2;
	static constexpr int MESSAGE_Y = 22;

	static constexpr int TIMER_WIDTH = 25;
	static constexpr int TIMER_X = (SCREEN_WIDTH - 1) - TIMER_WIDTH;
	static constexpr int TIMER_Y1 = MESSAGE_Y;
	static constexpr int TIMER_Y2 = MESSAGE_Y + 1;

	static constexpr char TORCH_CHAR = '!';
	static constexpr char OBSTACLE_CHAR = '*';
	static constexpr char DOOR_MIN = '1';
	static constexpr char DOOR_MAX = '9';
	static constexpr int NUM_DIRS = 4;
	static const Direction dirs[NUM_DIRS];

	// Default constructor
	Room();

	void drawRoom() const;
	void drawDarkRoom();
	void placeTorchAt(const Point& p);

	bool isWall(const Point& p) const;// Checks if the point is a wall in the room layout.
	bool isDoor(const Point& p) const;// Checks if the point is a door in the room layout.
	int tryOpenDoor(int doorNum, int playerKeyPass);// Return 0 if door cannot be opened, 1 if opened without key, 2 if opened with key.
	void placeDoorAt(int doorId, const Point& doorPos, bool needsKey, const Point& keyPos, int targetRoom, const std::vector<int>& switchIds);
	const char* getRow(int y) const { return layout[y]; }// Access to a specific row of the room layout.
	
	bool loadFromFile(const std::string& filename);
	void buildObjectsFromLayout();
	void parseDoorLine(const std::string& line);
	bool hasLegendPos() const { return hasLegend; }
	Point getLegendPos() const { return legendPos; }

	//ITEMS function:
	//-----------------------------------------------//
	DoorManager& getDoorManager() { return doorManager; }// Access to the room's door manager.
	KeyManager& getKeyManager() { return keyManager; }// Access to the room's key manager.

	Switch* getSwitchByID(int sId);

	void addSpring(const Spring& s);
	Spring* getSpringAt(const Point& p);
	void drawSpring(bool shouldDrawDark);
	void parseSpringLine(const std::string& line);
	void parseSwitchLine(const std::string& line);
	void placeSpringAt(const Spring& s);
	void placeSwitchAt(const Switch& s);

	void updateBombs();
	void handleExplosionAt(const Point& center, int radius);
	void placeBombAt(const Point& p, char ownerSymbol);

	bool getIsDarkRoom() const { return isDarkRoom; }

	void placeKeyAt(const Point& p, int pass);

	void addSwitch(const Switch& s);
	Switch* getSwitchAt(const Point& p);
	void changeSwitchAt(const Point& p);

	void printMessage(const char* msg);
	void clearMessage();
	void printDarkTimer(int secLeft);
	void clearDarkTimer();

	void checkSwitchOpenDoor();

	// Returns the character at a specific point in the room layout.
	char getCell(const Point& p) const {
		return layout[p.getY()][p.getX()];
	}

	// Clears a cell in the room layout and updates the display.
	void clearCell(const Point& p) {
		int x = p.getX();
		int y = p.getY();

		layout[y][x] = ' ';

		setTextColor(Color::WHITE);
		gotoxy(x, y);
		std::cout << ' ';
	}

	//Obstacle functions:
	void updateObstacles();
	bool startPushObstacleAt(const Point& hitCell, Direction dir, int appliedForce);
	void setCellChar(const Point& p, char c);
	void CollectObstacle(int x, int y, std::vector<std::vector<bool>>& visited, std::vector<Point>& outCells);

	//Event functions:
	void pushEvent(const GameEvent& e) {
		eventsThisFrame.push_back(e);
	}

	std::vector<GameEvent> consumeEvents() {
		auto out = eventsThisFrame;
		eventsThisFrame.clear();
		return out;
	}

	// Checks if the given coordinates are outside the defined game area.
	bool isOutsideGameArea(int x, int y) const {
		return !inRange(x, GAME_AREA_LEFT, GAME_AREA_RIGHT) ||
			!inRange(y, GAME_AREA_TOP, GAME_AREA_BOTTOM);
	}

	static bool inRange(int v, int min, int max) {
		return v >= min && v <= max;
	}


	bool getIsLoaded() {
		return isLoaded;
	}

	int getRoomID() {
		return id;
	}

	void setRoomID(int new_id) {
		id = new_id;
	}

};




