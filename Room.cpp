#define _CRT_SECURE_NO_WARNINGS
#include "Room.h"
#include "Game.h"
#include "utils.h"
#include "DoorManager.h"
#include "KeyManager.h"
#include <cstring>
#include "Player.h"
#include <iostream>
#include <fstream>
#include <sstream> 

using std::cout;
using std::endl;
using std::max;

// Direction array for room navigation.
const Direction Room::dirs[Room::NUM_DIRS] = {
	Direction::RIGHT,
	Direction::LEFT,
	Direction::DOWN,
	Direction::UP
};

// Default constructor initializes room properties and layout.
Room::Room()
	: id(-1),
	isDarkRoom(false)
{
	for (int row = 0; row < SCREEN_HEIGHT; ++row) {
		std::fill(layout[row], layout[row] + SCREEN_WIDTH, ' ');
		layout[row][SCREEN_WIDTH] = '\0';
	}

	bombs.clear();
	switches.clear();
	riddles.clear();
	springs.clear();
	obstacles.clear();
	eventsThisFrame.clear();
	doorManager.clear();
	keyManager.clear();
}

// Loads the room layout and objects from a specified file.
bool Room::loadFromFile(const std::string& filename) {
	if (isLoaded) return true;
	std::ifstream f(filename);
	if (!f.is_open()) return false;

	isDarkRoom = false;

	for (int row = 0; row < SCREEN_HEIGHT; ++row) {
		std::fill(layout[row], layout[row] + SCREEN_WIDTH, ' ');
	}

	bool foundP1 = false;
	bool foundP2 = false;
	hasP1 = hasP2 = hasLegend = false;

	int curr_row = 0;
	int curr_col = 0;
	char c;
	std::string line;
	bool readingLayout = true;

	int lineCount = 0;
	while (std::getline(f, line)) {
		lineCount++;
		if (curr_row < SCREEN_HEIGHT) {
			for (int curr_col = 0; curr_col < (int)line.length() && curr_col < SCREEN_WIDTH; ++curr_col) {
				char c = line[curr_col];

				if (c == '$') {
					p1Start = Point(curr_col, curr_row);
					foundP1 = true;
					hasP1 = true;
					layout[curr_row][curr_col] = ' ';
				}
				else if (c == '&') {
					p2Start = Point(curr_col, curr_row);
					foundP2 = true;
					hasP2 = true;
					layout[curr_row][curr_col] = ' ';
				}
				else if (c == 'L') {
					legendPos = Point(curr_col, curr_row);
					hasLegend = true;
					layout[curr_row][curr_col] = ' ';
				}
				else {
					layout[curr_row][curr_col] = c;
				}
			}
			curr_row++;
			continue;
		}

		if (line.empty()) continue;

		// The lines after the layout are for objects like doors, keys....
		if (curr_row >= SCREEN_HEIGHT) {
			if (line.find("door") != std::string::npos) {
				parseDoorLine(line);
			}
			if (line.find("spring") != std::string::npos) {
				parseSpringLine(line);
			}
			if (line.find("switch") != std::string::npos) {
				parseSwitchLine(line);
			}
			if (line == "dark") {
				isDarkRoom = true;
			}
			if (line.find("End information.") != std::string::npos) {
				break;
			}
		}

	}

	if (curr_row < SCREEN_HEIGHT) return false;
	if (!foundP1 && !foundP2) {
		hasP1 = hasP2 = false;
	}
	isLoaded = true;
	return true;
}

// Builds room objects like bombs and obstacles based on the current layout.
void Room::buildObjectsFromLayout() {
	std::vector<std::vector<bool>> visited(SCREEN_HEIGHT, std::vector<bool>(SCREEN_WIDTH, false));

	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			char ch = layout[y][x];

			if (ch == BOMB) {// Bomb character found
				bombs.emplace_back(Point(x, y, 0, 0, '@'), 3, '\0');
				layout[y][x] = BOMB;
			}

			else if (ch == OBSTACLE_CHAR && !visited[y][x]) {// Obstacle character found
				std::vector<Point> cells;
				CollectObstacle(x, y, visited, cells);

				if (!cells.empty()) {
					obstacles.emplace_back(cells);

					for (const Point& p : cells) {
						layout[p.getY()][p.getX()] = OBSTACLE_CHAR;
					}
				}
			}
		}
	}
}


// Draws the room layout with appropriate colors for players and items.
void Room::drawRoom() const {
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			char c = layout[y][x];

			if (c == Bomb::BOMB_CHAR) {
				setTextColor(Color::RED);
			}

			else if (c == Room::TORCH_CHAR) {
				setTextColor(Color::YELLOW);
			}
			else if (c == Riddle::RIDDLE_CHAR) {
				setTextColor(Color::GREEN);
			}
			else if (c == Room::OBSTACLE_CHAR) {
				setTextColor(Color::MAGENTA);
			}
			else if (c == Spring::SPRING_CHAR) {
				setTextColor(Color::CYAN);
			}

			else {
				setTextColor(Color::WHITE);
			}

			std::cout << c;// Print the character at the current position.
		}
		std::cout << std::endl;
	}
	setTextColor(Color::WHITE);// Reset text color to white after drawing the room.
}


// Draws a darkened version of the room, hiding non-wall and non-door elements.
void Room::drawDarkRoom() {
	setTextColor(Color::DARK_GRAY);

	for (int y = 0; y < SCREEN_HEIGHT; ++y) {

		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			char c = layout[y][x];
			char outChar = c;
			Point p(x, y, 0, 0, ' ');

			if (!isOutsideGameArea(x, y)) {
				bool isFrameWall = (c == '-' || c == '|');

				if (c != ' ' && !isFrameWall) {
					outChar = ' ';
				}
			}

			cout << outChar;
		}
		cout << endl;
	}

	setTextColor(Color::WHITE);
}


// Checks if the point corresponds to a wall in the room layout.
bool Room::isWall(const Point& p) const {
	char c = layout[p.getY()][p.getX()];
	return (c == 'w' || c == 'W' || c == '-' || c == '|' || c == '_');
}

// Checks if the point corresponds to a door in the room layout.
bool Room::isDoor(const Point& p) const {
	char c = layout[p.getY()][p.getX()];
	return DoorManager::isDoorChar(c);
}

// Attempts to open a door based on the player's key password and the door's id.
int Room::tryOpenDoor(int doorNum, int playerKeyPass) {
	Door* d = doorManager.getDoorByID(doorNum);
	if (!d) return 0;

	if (!d->reqKey() || d->open())// Door does not require a key
		return 1;

	if (doorNum == 2)
		//int x = 5;

		if (playerKeyPass == -1)// Player does not have a key
			return 0;

	if (d->getID() == playerKeyPass) {// Dor open with the right key
		d->setOpen(true);
		return 2;
	}

	return 0;
}

// Updates the timers of all active bombs and handles explosions.
void Room::updateBombs() {
	for (Bomb& b : bombs) {
		if (!b.isActiveNow())
			continue;

		b.decreaseTimer();

		if (b.shouldExplode()) {
			pushEvent({ GameEventType::Explosion, b.getLocation(), b.getRadius(), b.getOwner() });
			handleExplosionAt(b.getLocation(), b.getRadius());
			b.deactivate();
		}
	}
}

// Handles the explosion effect at a given center point with a specified radius.
void Room::handleExplosionAt(const Point& center, int radius) {
	int cx = center.getX();
	int cy = center.getY();

	for (int dy = -radius; dy <= radius; ++dy) {
		for (int dx = -radius; dx <= radius; ++dx) {
			int x = cx + dx;
			int y = cy + dy;

			if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
				continue;

			if (x == 0 || x == SCREEN_WIDTH - 1)
				continue;

			if (y < GAME_AREA_TOP || y > GAME_AREA_BOTTOM)
				continue;

			char& cell = layout[y][x];

			if (cell != ' ') {
				cell = ' ';
				gotoxy(x, y);
				std::cout << ' ';
			}
		}
	}
}

// Places a key at the specified point in the room layout.
void Room::placeKeyAt(const Point& p, int pass) {
	int x = p.getX();
	int y = p.getY();

	if (layout[y][x] == KeyManager::KEY_CHAR) {
		keyManager.addKey(Key(p, KeyManager::KEY_CHAR, pass)); // Add the key to the key manager.
		return;
	}

	// Place the key in the room layout and display it.
	layout[y][x] = KeyManager::KEY_CHAR;
	gotoxy(x, y);
	std::cout << KeyManager::KEY_CHAR;

	keyManager.addKey(Key(p, KeyManager::KEY_CHAR, pass)); // Add the key to the key manager.
};


// Places a Door at the specified point in the room layout.
void Room::placeDoorAt(int doorId, const Point& doorPos, bool needsKey, const Point& keyPos, int targetRoom, const std::vector<int>& switchIds) {
	int x = doorPos.getX();
	int y = doorPos.getY();
	char doorChar = doorId + '0';


	// Create the door object
	Door d(doorId, doorPos, needsKey, keyPos, targetRoom);
	for (int sId : switchIds) {
		d.addRequiredSwitch(sId);
	}

	doorManager.addDoor(d);

	if (!switchIds.empty()) {
		d.setOpen(false);
	}

	if (layout[y][x] == doorChar) {
		return;
	}

	// Place the door in the room layout and display it.
	layout[y][x] = doorChar;
	gotoxy(x, y);
	std::cout << doorChar;
}


// Places a bomb at the specified point in the room layout.
void Room::placeBombAt(const Point& p, char ownerSymbol) {
	Bomb b;
	b.placeAt(p);
	b.setOwner(ownerSymbol);
	b.activate();
	bombs.push_back(b);
}


// Places a torch at the specified point in the room layout.
void Room::placeTorchAt(const Point& p) {
	int x = p.getX();
	int y = p.getY();

	if (layout[y][x] == TORCH_CHAR)
		return;

	layout[y][x] = TORCH_CHAR;

	setTextColor(Color::YELLOW);
	gotoxy(x, y);
	std::cout << TORCH_CHAR;
	setTextColor(Color::WHITE);
}

// Places a spring at the specified points in the room layout.
void Room::placeSpringAt(const Spring& s) {
	for (const Point& p : s.getPoints()) {
		int x = p.getX();
		int y = p.getY();

		layout[y][x] = Spring::SPRING_CHAR;
		gotoxy(x, y);
		std::cout << Spring::SPRING_CHAR;
	}

	addSpring(s);
}

// Places a switch at the specified point in the room layout.
void Room::placeSwitchAt(const Switch& s) {
	int x = s.getLocation().getX();
	int y = s.getLocation().getY();

	char displayChar = s.getChar();

	if (layout[y][x] == displayChar) {
		addSwitch(s);
		return;
	}

	layout[y][x] = displayChar;
	gotoxy(x, y);
	std::cout << displayChar;

	addSwitch(s);
}

// Adds a switch to the room's switch list.
void Room::addSwitch(const Switch& s) {
	switches.push_back(s);
}

// Retrieves a pointer to the switch located at the specified point.
Switch* Room::getSwitchAt(const Point& p) {
	for (Switch& s : switches) {
		if (s.getLocation().getX() == p.getX() &&
			s.getLocation().getY() == p.getY())
			return &s;
	};
	return nullptr;
}

// Changes the state of the switch at the specified point.
void Room::changeSwitchAt(const Point& p) {
	Switch* sw = getSwitchAt(p);
	if (!sw) {
		return;
	}
	sw->change();

	int x = p.getX();
	int y = p.getY();

	if (layout[y][x] == Switch::SWITCH_OF_CHAR) {
		layout[y][x] = Switch::SWITCH_ON_CHAR;
		gotoxy(x, y);
		std::cout << Switch::SWITCH_ON_CHAR;
	}

	else if (layout[y][x] == Switch::SWITCH_ON_CHAR) {
		layout[y][x] = Switch::SWITCH_OF_CHAR;
		gotoxy(x, y);
		std::cout << Switch::SWITCH_OF_CHAR;
	}

	checkSwitchOpenDoor();
	return;
}


// Prints a message in the console.
void Room::printMessage(const char* msg) {
	const int msgX = MESSAGE_X;
	const int msgY = MESSAGE_Y;

	const int msgWidth = TIMER_X - msgX;

	gotoxy(msgX, msgY);
	setTextColor(Color::YELLOW);

	std::string s = msg ? msg : "";
	if ((int)s.size() > msgWidth) s.resize(msgWidth);

	std::cout << s;

	for (int i = (int)s.size(); i < msgWidth; ++i)
		std::cout << ' ';

	setTextColor(Color::WHITE);
}

// Clears the message area in the console.
void Room::clearMessage() {
	gotoxy(MESSAGE_X, MESSAGE_Y);
	setTextColor(Color::YELLOW);

	std::cout << "                                        "; // Clear previous message
}

// Prints the dark timer with the remaining seconds.
void Room::printDarkTimer(int secLeft) {
	setTextColor(Color::YELLOW);

	gotoxy(TIMER_X, TIMER_Y1);
	std::string l1 = "You need to get the torch!";
	if ((int)l1.size() > TIMER_WIDTH) l1.resize(TIMER_WIDTH);
	std::cout << l1;
	for (int i = (int)l1.size(); i < TIMER_WIDTH; ++i) std::cout << ' ';

	gotoxy(TIMER_X, TIMER_Y2);
	std::string l2 = "Time left: " + std::to_string(secLeft) + "s";
	if ((int)l2.size() > TIMER_WIDTH) l2.resize(TIMER_WIDTH);
	std::cout << l2;
	for (int i = (int)l2.size(); i < TIMER_WIDTH; ++i) std::cout << ' ';

	setTextColor(Color::WHITE);
}

// Clears the dark timer area in the console.
void Room::clearDarkTimer() {
	gotoxy(TIMER_X, TIMER_Y1);
	for (int i = 0; i < TIMER_WIDTH; ++i) std::cout << ' ';

	gotoxy(TIMER_X, TIMER_Y2);
	for (int i = 0; i < TIMER_WIDTH; ++i) std::cout << ' ';
}

// Retrieves a pointer to the switch with the specified ID.
Switch* Room::getSwitchByID(int sId) {
	for (auto& sw : switches) {
		if (sw.getId() == sId) {
			return &sw; // Found the switch with the matching ID
		}
	}
	return nullptr;
}


// Checks the state of switches and opens/closes doors accordingly.
void Room::checkSwitchOpenDoor() {
	// Check each door in the room
	for (auto& door : doorManager.getDoors()) {

		// Not relevant door if it has no required switches
		if (door.getRequiredSwitches().empty()) {
			continue;
		}

		bool allSwitchesOn = true;

		// Check each required switch for the door
		for (int sId : door.getRequiredSwitches()) {
			Switch* sw = getSwitchByID(sId); // Find the switch by its ID
			bool check = sw->isActivated();
			if (sw == nullptr || !sw->isActivated()) {
				allSwitchesOn = false;
				break; // If any switch is off, no need to check further
			}
		}

		// Find door position and character
		int x = door.getPosition().getX();
		int y = door.getPosition().getY();
		char doorChar = door.getID() + '0';

		if (allSwitchesOn) {
			door.setOpen(true);
		}
		else {
			door.setOpen(false);
		}
	}
}

// Adds a spring to the room's spring list.
void Room::addSpring(const Spring& s) {
	springs.push_back(s);
}

// Retrieves a pointer to the spring located at the specified point.
Spring* Room::getSpringAt(const Point& p) {
	for (auto& spring : springs) {
		const Point& loc = spring.getLocation();
		if (loc.getX() == p.getX() && loc.getY() == p.getY()) {
			return &spring;
		}
	}
	return nullptr;
}

// Updates the position based on direction character and offset.
void Room::drawSpring(bool shouldDrawDark) {
	for (const Spring& s : springs) {
		for (const Point& p : s.getPoints()) {
			int x = p.getX();
			int y = p.getY();

			layout[y][x] = Spring::SPRING_CHAR;

			if (shouldDrawDark && !isOutsideGameArea(x, y)) {
				continue;
			}

			setCellChar(Point(x, y), Spring::SPRING_CHAR);
		}
	}
}

// Sets the character at a specific point in the room layout and updates the display.
void Room::setCellChar(const Point& p, char c) {
	int x = p.getX();
	int y = p.getY();
	layout[y][x] = c;

	if (c == OBSTACLE_CHAR) {
		setTextColor(Color::MAGENTA);
	}
	else if (c == Spring::SPRING_CHAR) {
		setTextColor(Color::CYAN);
	}
	else if (c == Riddle::RIDDLE_CHAR) {
		setTextColor(Color::GREEN);
	}
	else if (c == Bomb::BOMB_CHAR) {
		setTextColor(Color::RED);
	}
	else if (c == TORCH_CHAR) {
		setTextColor(Color::YELLOW);
	}
	else {
		setTextColor(Color::WHITE);
	}

	gotoxy(x, y);
	std::cout << c;

	setTextColor(Color::WHITE);
}

// Builds obstacles from the room layout by scanning for obstacle characters.
void Room::updateObstacles() {
	for (Obstacle& o : obstacles) {
		if (o.isMoving()) {
			o.update(*this);
		}
	}
}

// Attempts to start pushing an obstacle at the specified hit cell in a given direction with applied force.
bool Room::startPushObstacleAt(const Point& hitCell, Direction dir, int appliedForce) {
	for (Obstacle& o : obstacles) {
		if (!o.contains(hitCell)) continue;

		return o.startPush(*this, dir, appliedForce);
	}
	return false;
}

// Parses a door line from the room file and updates the global room maps.
void Room::parseDoorLine(const std::string& line) {
	int doorId = -1, dX = -1, dY = -1, targetRoom = -1, kX = -1, kY = -1;
	std::vector<int> switchIds;

	// Our format of text: door %d (%d,%d) to room: %d. .....
	int count = sscanf(line.c_str(), "door %d (%d,%d) to room: %d", &doorId, &dX, &dY, &targetRoom);

	if (count >= 4) {

		// Door need a key
		if (line.find("with key:") != std::string::npos) {
			if (sscanf(line.substr(line.find("with key:")).c_str(), "with key: (%d, %d)", &kX, &kY) == 2) {
				placeKeyAt(Point(kX, kY, 0, 0, 'K'), doorId);
			}
		}

		size_t switchesPos = line.find("switches:");

		// Door controlled by switches
		if (switchesPos != std::string::npos) {
			std::string idsPart = line.substr(switchesPos + 9);
			for (char& c : idsPart) {
				if (!isdigit(c)) {
					c = ' ';
				}
			}

			std::stringstream ss(idsPart);
			int sId;

			while (ss >> sId) {
				switchIds.push_back(sId);
			}
		}

		Point doorPos(dX, dY, 0, 0, (char)(doorId + '0'));
		bool needsKey = (kX != -1 && kY != -1);
		Point keyPos = needsKey ? Point(kX, kY) : Point(-1, -1);
		bool isLocked = needsKey || !switchIds.empty();

		placeDoorAt(doorId, doorPos, isLocked, keyPos, targetRoom, switchIds);
	}
}

// Parses a spring line from the room file and places the spring in the room.
void Room::parseSpringLine(const std::string& line) {
	int startX, startY, length;
	char dirChar;

	// Found the format line of a spring
	if (sscanf(line.c_str(), "spring (%d,%d) enter dir: %c length: %d",
		&startX, &startY, &dirChar, &length) == 4) {

		// verctor of points for this spring 
		std::vector<Point> pts;
		pts.reserve(length);

		for (int i = 0; i < length; ++i) {
			int curX = startX;
			int curY = startY;

			// Check direction and update current position
			updatePositionByDir(dirChar, i, curX, curY);

			// Add each point of the spring to the vector
			pts.emplace_back(curX, curY, 0, 0, Spring::SPRING_CHAR);
		}

		// Get enter and exit directions
		Direction enter = charToDirection(dirChar);
		Direction exit = charToDirection(getOppositeChar(dirChar));

		placeSpringAt(Spring(pts, enter, exit));
	}
}

// Parses a switch line from the room file and places the switch in the room.
void Room::parseSwitchLine(const std::string& line) {
	int sId, x, y;
	char stateStr[10];

	// Found the format line of a spring
	if (sscanf(line.c_str(), "switch %d (%d,%d) state: %s", &sId, &x, &y, stateStr) == 4) {

		bool startOn = (std::string(stateStr) == "ON");
		char initialChar = startOn ? Switch::SWITCH_ON_CHAR : Switch::SWITCH_OF_CHAR;

		Point p(x, y, 0, 0, initialChar);
		Switch newSwitch(sId, p, startOn);


		//мозеч??
		if (startOn) newSwitch.change();// Switch started off, set to ON

		placeSwitchAt(newSwitch);
	}
}

// Recursively collects all connected obstacle cells starting from (x, y).
void Room::CollectObstacle(int x, int y,
	std::vector<std::vector<bool>>& visited,
	std::vector<Point>& outCells)
{
	if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
		return;

	if (visited[y][x])
		return;

	if (layout[y][x] != OBSTACLE_CHAR)
		return;

	visited[y][x] = true;

	outCells.emplace_back(Point(x, y, 0, 0, OBSTACLE_CHAR));

	CollectObstacle(x + 1, y, visited, outCells);
	CollectObstacle(x - 1, y, visited, outCells);
	CollectObstacle(x, y + 1, visited, outCells);
	CollectObstacle(x, y - 1, visited, outCells);
}




