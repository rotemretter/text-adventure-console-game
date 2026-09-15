#include "Game.h"
#include "Room.h"
#include "utils.h"
#include "finalRoom.h"

using std::cout;
using std::endl;

Game::Game(bool save, bool load, bool silent) :
	isRunning(true),
	inGame(false),
	rooms(),
	isSaveMode(save),
	isLoadMode(load),
	isSilentMode(silent),
	gameCycle(0),
	currentRoomIndex(0) {
	roomFiles = getAllRoomFileNames();

	for (const auto& f : roomFiles) {
		rooms.emplace_back();
		rooms.back().loadFromFile(f);
		rooms.back().buildObjectsFromLayout();
	}

	players[0] = Player(
		Point(5, 5, 1, 0, '$'),
		"wdxas",
		&rooms[currentRoomIndex]);

	players[1] = Player(
		Point(70, 15, 1, 0, '&'),
		"ilmjk",
		&rooms[currentRoomIndex]);

	players[0].setGame(this);
	players[1].setGame(this);

}

// GameLoad constructor: initializes the game in load mode, loading steps and results from files, and setting the random seed accordingly.
GameLoad::GameLoad(bool silent) : Game(false, true, silent), isSilent(silent) {
	steps.loadSteps("adv-world.steps");
	results.loadResults("adv-world.result");

	srand(steps.getRandomSeed());
}

// GamePlay constructor: initializes a new game session, generating a random seed for gameplay.
GamePlay::GamePlay(bool save) : Game(save, false, false) {
	// For new game sessions, we generate a new random seed based on the current time.
	randomSeed = static_cast<long>(time(NULL));
	srand(randomSeed);

	// Set the random seed in the Steps object for logging purposes.
	steps.setRandomSeed(randomSeed);
}


// Scans the current working directory for room screen files , sorts them lexicographically, and returns the list of filenames.
std::vector<std::string> Game::getAllRoomFileNames() {
	namespace fs = std::filesystem;
	std::vector<std::string> files;

	for (const auto& entry : fs::directory_iterator(fs::current_path()))
	{
		if (!entry.is_regular_file()) continue;

		auto path = entry.path();
		auto ext = path.extension().string();
		auto name = path.filename().string();

		if (ext == ".screen" && name.rfind("room", 0) == 0) {
			files.push_back(name);
		}
	}

	std::sort(files.begin(), files.end());
	return files;
}

//Source: ChatGPT
//Displays the main menu and handles the user's selection
void Game::mainMenu() {
	cls();
	setTextColor(Color::WHITE);
	std::cout << "============================================================\n";
	std::cout << "                 WELCOME TO THE WORLD OF \n";
	std::cout << "============================================================\n\n";

	std::cout << "    _     ____ __     __ _____  _   _ _______ _   _ _____   _____    \n";
	std::cout << "   / \\   |  _ \\\\ \\   / /| ____|| \\ | |  | |  | | | ||  _ \\ | ____|   \n";
	std::cout << "  / _ \\  | | | |\\ \\ / / |  _|  |  \\| |  | |  | | | || |_) ||  _|     \n";
	std::cout << " / ___ \\ | |_| | \\ V /  | |___ | |\\  |  | |  | | | ||  _ < | |___    \n";
	std::cout << "/_/   \\_\\|____/   \\_/   |_____||_| \\_|  |_|  |_|_|_||_| \\_\\|_____|   \n\n";


	std::cout << "======================  MENU  ==============================\n";
	std::cout << "(1) Start a new Game\n";
	std::cout << "(8) Instructions & Keys\n";
	std::cout << "(9) Exit Game\n";
	std::cout << "============================================================\n";
	std::cout << "Choose your destiny: ";

	char choice;
	std::cin >> choice;

	// Handle menu choice
	switch (static_cast<MenuChoice>(choice)) {// Cast to MenuChoice enum
	case MenuChoice::Start:
		char c;
		std::cout << "Run with colors? (y/n): ";
		std::cin >> c;
		if (c == 'y' || c == 'Y')
			setColorsEnabled(true);
		else
			setColorsEnabled(false);

		currentRoomIndex = FIRST_ROOM_INDEX;
		startNewGame();
		break;

	case MenuChoice::Instructions:
		showInstructions();
		break;

	case MenuChoice::Exit:
		isRunning = false;
		cls();
		break;
	}
}


// Runs the application (menu loop)
void GamePlay::run() {
	while (isRunning) {
		mainMenu();
	}
}

void GameLoad::run() {
	startNewGame();
}

// Initializes a new game session: loads all room files, builds room objects, initializes players and riddles, then enters the main game loop.
void Game::startNewGame() {
	currentRoomIndex = FIRST_ROOM_INDEX;

	rooms.clear();
	rooms.reserve(roomFiles.size());

	int idCounter = 1;
	for (const std::string& filename : roomFiles) {
		rooms.emplace_back();

		if (!rooms.back().loadFromFile(filename)) {
			bool backToMenu = showInvalidRoomScreen(filename);
			if (!backToMenu) {
				std::exit(0);
			}
			return;
		}

		rooms.back().buildObjectsFromLayout();
		rooms.back().setRoomID(idCounter);
		idCounter++;
	}

	if (rooms.empty()) {
		std::cerr << "Error: No rooms were loaded!" << std::endl;
		return;
	}

	if (currentRoomIndex >= rooms.size()) {
		std::cerr << "Error: currentRoomIndex (" << currentRoomIndex << ") is out of bounds!" << std::endl;
		return;
	}

	cls();
	invalidateHud();
	rooms[currentRoomIndex].drawRoom();

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		players[i].setRoom(&rooms[currentRoomIndex]);
	}

	initPlayers();
	if (!riddleBank.loadRiddleFromFile("riddles.txt")) {
		popupMessageTimed("ERROR", { "Missing or invalid riddles.txt" }, 3000);
		return;
	}
	initSeed();
	inGame = true;

	gameCycle = 0; // Reset game cycle count

	gameLoop();
}

void GamePlay::initSeed() {
	randomSeed = static_cast<long>(std::chrono::system_clock::now().time_since_epoch().count());
	if (isSaveMode) {
		steps.setRandomSeed(randomSeed);
	}
	srand(randomSeed);
}

void GameLoad::initSeed() {
	srand(randomSeed);
}


// Initializes players for a brand new game (full reset of scores/lives/items and initial positions).
void Game::initPlayers() {
	setupPlayers(true);
	return;
}

// Resets player positions/state when entering a new room (without resetting the entire game progress).
void Game::initInRoom() {
	setupPlayers(false);
	return;
}

// Internal helper to (re)position and reset players either for a new game or when entering a room, including basic validation of room state.
void Game::setupPlayers(bool isNewGame) {
	// Deal with error cases first
	if (rooms.empty()) {
		std::cerr << "[ERROR] Critical: No rooms loaded in the game!" << std::endl;
		inGame = false;
		return;
	}

	if (currentRoomIndex < 0 || currentRoomIndex >= (int)rooms.size()) {
		std::cerr << "[ERROR] Invalid currentRoomIndex: " << currentRoomIndex
			<< ". Out of bounds!" << std::endl;
		inGame = false;
		return;
	}

	// Now proceed with normal setup
	Point startPoints[NUM_PLAYERS] = {
		Point(5, 5, 1, 0, '$'),
		Point(20, 15, 1, 0, '&')
	};


	for (int i = 0; i < NUM_PLAYERS; ++i) {
		players[i].setId(i);
		players[i].setDisappeared(false);
		players[i].setRoom(&rooms[currentRoomIndex]);
		if (isNewGame) {
			players[i].resetForNewGame(startPoints[i]);
		}
		else {
			players[i].resetInRoom(startPoints[i]);
		}
	}
}

// Displays a full-screen riddle prompt, collects a single choice, and returns true if the answer is correct.
bool Game::showRiddleFullScreen(const Riddle& r, std::string& userAnswer)
{
	cls();
	setTextColor(Color::WHITE);

	std::cout << "=========================== RIDDLE ===========================\n\n";

	std::cout << r.getQuestion() << "\n\n";

	const vector<string>& opt = r.getOptions();
	std::cout << "A) " << opt[0] << "\n";
	std::cout << "B) " << opt[1] << "\n";
	std::cout << "C) " << opt[2] << "\n";
	std::cout << "D) " << opt[3] << "\n\n";

	std::cout << "Choose: A/B/C/D (or 1/2/3/4)\n";

	int chosen = -1;
	char k;
	while (chosen == -1) {
		k = _getch();

		if (k == 'A' || k == 'a' || k == '1') chosen = 0;
		else if (k == 'B' || k == 'b' || k == '2') chosen = 1;
		else if (k == 'C' || k == 'c' || k == '3') chosen = 2;
		else if (k == 'D' || k == 'd' || k == '4') chosen = 3;
	}

	userAnswer = std::string(1, k);

	return (chosen == r.getCorrectIndex());
}

// Clears and redraws the current room (dark/non-dark), redraws all visible players, and refreshes the HUD.
void Game::redrawCurrentRoom() {
	cls();
	invalidateHud();

	Room& room = rooms[currentRoomIndex];

	bool anyTorch = false;
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		anyTorch |= players[i].hasTorchItem();
	}
	bool shouldDrawDark = room.getIsDarkRoom() && !anyTorch;

	if (shouldDrawDark) {
		room.drawDarkRoom();
	}
	else {
		room.drawRoom();
	}

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		if (!players[i].hasDisappeared()) {
			players[i].draw();
		}
	}

	drawStatusBar();
	hudInited = true;
}

// Displays instructions and control keys to the player
void Game::showInstructions() {
	cls();
	setTextColor(Color::WHITE);
	std::cout << "Instructions:\n";
	std::cout << "Goal:\n" << "Get both players to the final room.\n";
	std::cout << " \n";
	std::cout << "Player 1 controls (represented by $):\n";
	std::cout << " W - Up\n S - Down\n A - Left\n D - Right\n X - Stay\n E - Dispose Element\n";
	std::cout << " \n";
	std::cout << "Player 2 controls (represented by &):\n";
	std::cout << " I - Up\n K - Down\n J - Left\n L - Right\n M - Stay\n O - Dispose Element\n";
	std::cout << " \n";
	std::cout << "Game Control:\n";
	std::cout << "ESC - Pause game\n" << "ESC again - Resume\n" << "H - Return to menu from pause\n";
	std::cout << " \n";
	std::cout << "Legend:\n" << "$ & -Players\n" << "@ -Bomb\n" << "! -Torch\n" << "K -Key\n" << "/ -Switch\n" << "1,2 -Doors\n";
	std::cout << " \n";
	std::cout << "\nPress any key to return to menu";
	_getch();
}

// Resets HUD cache state so the next drawStatusBar call will fully redraw items/scores/lives.
void Game::invalidateHud() {
	hudInited = false;
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		lastLivesShown[i] = -1;
		lastScoreShown[i] = -1;
		lastItemShown[i] = NONE_ITEM;
	}
}

// Draws the status bar showing players' items
void Game::drawStatusBar() {
	const bool isFinalRoom = !rooms.empty() && currentRoomIndex == (int)rooms.size();
	if (isFinalRoom) return;

	Room& room = rooms[currentRoomIndex];

	bool anyTorch = false;
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		anyTorch |= players[i].hasTorchItem();
	}

	bool shouldDrawDark = room.getIsDarkRoom() && !anyTorch;
	setTextColor(shouldDrawDark ? Color::DARK_GRAY : Color::WHITE);

	int baseX = 1, baseY = 1;
	if (room.hasLegendPos()) {
		Point L = room.getLegendPos();
		baseX = L.getX();
		baseY = L.getY();
	}

	if (!hudInited) {
		gotoxy(baseX - 1, baseY);
		cout << "|PLAYER $: ITEMS:            LIVES:   | PLAYER &: ITEMS:            LIVES:     |";

		gotoxy(baseX - 1, baseY + 1);
		cout << "|__________________SCORE: 0   ________|___________________SCORE: 0   __________|";
	}

	const int firstColX = (baseX - 1) + 19;
	const int colStep = 39;

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		setTextColor(shouldDrawDark ? Color::DARK_GRAY : Color::WHITE);
		int x = firstColX + i * colStep;

		ItemType currItem = NONE_ITEM;
		if (players[i].hasBombItem())      currItem = BOMB;
		else if (players[i].hasTorchItem()) currItem = TORCH;
		else if (players[i].hasKeyItem())   currItem = KEY;

		if (!hudInited || currItem != lastItemShown[i]) {
			gotoxy(x, baseY);

			switch (currItem) {
			case BOMB:  cout << "BOMB   "; break;
			case TORCH: cout << "TORCH  "; break;
			case KEY:   cout << "KEY    "; break;
			default:    cout << "NONE   "; break;
			}

			lastItemShown[i] = currItem;
		}

		int currScore = players[i].getScore();
		if (!hudInited || currScore != lastScoreShown[i]) {
			gotoxy(x, baseY + 1);
			cout << "SCORE: " << currScore << "   ";
			lastScoreShown[i] = currScore;
		}

		int currLives = players[i].getLives();
		if (!hudInited || currLives != lastLivesShown[i]) {
			if (i == 0) printHearts(36, 1, currLives);
			else        printHearts(75, 1, currLives);
			lastLivesShown[i] = currLives;
		}
	}

	hudInited = true;
	setTextColor(Color::WHITE);
}


// Main loop of the game while a game session is active
void Game::gameLoop() {
	gameCycle = 0;
	while (inGame) {
		gameCycle++;
		rooms[currentRoomIndex].updateBombs();
		processRoomEvents();
		rooms[currentRoomIndex].updateObstacles();

		for (int i = 0; i < NUM_PLAYERS; ++i) {
			players[i].handleLaunch();
		}
		drawStatusBar();
		updateAndRedraw();
		handleInput();
		justStartedCarry = false;

		// Load mode doesn't need to wait
		Sleep(getSleepTime());
	}
	handleEndOfGame();
}

void GamePlay::handleEndOfGame() {
	if (isSaveMode) {
		int s0 = players[0].getScore();
		int s1 = players[1].getScore();
		int winnerScore = (s0 > s1) ? s0 : s1;
		results.addGameEnd(gameCycle, winnerScore);
		steps.saveSteps("adv-world.steps");
		results.saveResults("adv-world.result");
		std::cout << "Game saved successfully!" << std::endl;
	}
}

// Updates room lighting based on whether players have torches
void Game::updateLightingForCurrentRoom() {
	Room& room = rooms[currentRoomIndex];

	if (!room.getIsDarkRoom()) {
		lastDrawDark = false;
		return;
	}

	bool anyTorch = false;
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		anyTorch |= players[i].hasTorchItem();
	}

	bool shouldDrawDark = !anyTorch;

	if (shouldDrawDark != lastDrawDark) {
		lastDrawDark = shouldDrawDark;
		redrawCurrentRoom();
	}
}



// Updates player positions and redraws the game state
void Game::updateAndRedraw() {
	int requestedRoom = NO_ROOM_REQUEST;
	int requestedByPlayerId = -1;
	Room& room = rooms[currentRoomIndex];

	bool anyTorch = false;
	for (int j = 0; j < NUM_PLAYERS; ++j) {
		anyTorch |= players[j].hasTorchItem();
	}
	bool shouldDrawDark = room.getIsDarkRoom() && !anyTorch;

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		Player& p = players[i];

		if (p.hasDisappeared() || !p.isAlive())
			continue;

		Point oldPos = p.getPoint();

		MoveResult moveResult = p.move();

		if (moveResult.type == MoveResultType::Riddle) {
			runRiddleFlow(p, room, p.getPoint());
			moveResult.roomId = NO_ROOM_REQUEST;
		}

		Point newPos = p.getPoint();

		if (moveResult.type == MoveResultType::Door) {
			requestedRoom = moveResult.roomId;
			requestedByPlayerId = p.getId();
			p.setDisappeared(true);

			char bg = room.getCell(oldPos);
			gotoxy(oldPos.getX(), oldPos.getY());
			handleColor(bg);
			std::cout << bg;
		}
		else if (oldPos.getX() != newPos.getX() || oldPos.getY() != newPos.getY()) {

			char bg = room.getCell(oldPos);
			gotoxy(oldPos.getX(), oldPos.getY());

			char out = bg;
			if (shouldDrawDark && oldPos.getY() >= GAME_AREA_TOP && oldPos.getY() <= GAME_AREA_BOTTOM) {
				if (bg != ' ' && bg != '-' && bg != '|') {
					out = ' ';
					setTextColor(Color::DARK_GRAY);
				}
				else handleColor(out);
			}
			else handleColor(out);

			std::cout << out;

			if (!p.hasDisappeared() && p.isAlive()) {
				setTextColor(Color::BLUE);
				p.draw();
			}
		}
		else {
			if (!p.hasDisappeared() && p.isAlive()) {
				setTextColor(Color::BLUE);
				p.draw();
			}
		}

		setTextColor(Color::WHITE);
	}

	bool allGone = true;
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		allGone &= players[i].hasDisappeared();
	}

	if (requestedRoom != NO_ROOM_REQUEST && allGone) {
		if (requestedRoom == FINAL_ROOM_REQUEST) {
			showFinalScreen();
			inGame = false;
			return;
		}

		moveToRoom(requestedRoom, requestedByPlayerId);

		for (int i = 0; i < NUM_PLAYERS; ++i) {
			players[i].setDisappeared(false);
		}
	}

	updateLightingForCurrentRoom();
	updateDarkTimer();
}


// Runs the riddle interaction flow for a player: picks a riddle, shows it full-screen, updates score/lives based on correctness, clears the riddle cell, and redraws the room.
void Game::runRiddleFlow(Player& p, Room& room, const Point& oldPlayerPos)
{
	int ridIdx = riddleBank.pickRandomRiddleIndex();
	if (ridIdx == -1) {
		return;
	}

	const Riddle& rid = riddleBank.getRiddle(ridIdx);
	std::string userAnswer;
	bool correct = showRiddleFullScreen(rid, userAnswer);
	if (isSaveMode) {
		results.addRiddle(gameCycle, p.getId(), rid.getQuestion(), userAnswer, correct);
	}
	redrawCurrentRoom();

	if (correct) {
		p.addScore(200);
		Point qPos = p.getLastRiddlePos();
		riddleBank.markSolved(ridIdx);
		room.clearCell(qPos);
		p.setPoint(qPos);
		popupMessageTimed("RIDDLE", { "Nice!", "Correct answer.", "+200 points!" }, 1500);
		char under = room.getCell(oldPlayerPos);
		gotoxy(oldPlayerPos.getX(), oldPlayerPos.getY());
		handleColor(under);
		std::cout << under;
		setTextColor(Color::WHITE);

	}
	else {
		p.setPoint(oldPlayerPos);
		p.resetDirection();
		handlePlayerDeath(p);
		popupMessageTimed("RIDDLE", { "Oops...", "Wrong answer.", "You lost 1 life." }, 1500);
	}
	redrawCurrentRoom();
}

// Sets the console text color based on the cell character
void Game::handleColor(char cell) {
	if (cell == Bomb::BOMB_CHAR) setTextColor(Color::RED);
	else if (cell == Room::TORCH_CHAR) setTextColor(Color::YELLOW);
	else if (cell == Riddle::RIDDLE_CHAR) setTextColor(Color::GREEN);
	else if (cell == Room::OBSTACLE_CHAR) setTextColor(Color::MAGENTA);
	else if (cell == Spring::SPRING_CHAR) setTextColor(Color::CYAN);
	else setTextColor(Color::WHITE);
}

// Moves the game to a specified room by room ID, updating player states and room display accordingly.
void Game::moveToRoom(int roomID, int triggeringPlayerId) {
	int index = roomID - FIRST_ROOM_ID;

	if (index < 0 || index >= (int)roomFiles.size()) {
		std::cerr << "Error: Room ID " << roomID << " out of files range." << std::endl;
		return;
	}

	currentRoomIndex = index;
	Room& room = rooms[currentRoomIndex];

	if (isSaveMode) {
		results.addScreenChange(gameCycle, triggeringPlayerId, roomFiles[index]);
	}

	bool playerHasTorch = false;
	for (int i = 0; i < NUM_PLAYERS; i++) {
		playerHasTorch |= players[i].hasTorchItem();
	}

	cls();

	if (room.getIsDarkRoom() && !playerHasTorch) {
		room.drawDarkRoom();
	}
	else {
		room.drawRoom();
	}

	for (int i = 0; i < NUM_PLAYERS; i++) {
		players[i].setRoom(&room);
	}

	initInRoom();

	for (int i = 0; i < NUM_PLAYERS; i++) {
		if (!players[i].hasDisappeared())
			players[i].draw();
	}
	invalidateHud();
	drawStatusBar();
}

// לבדוק אותה -העתקתי מהצאט
void GameLoad::handleInput() {
	// בודקים אם בסיבוב הנוכחי (gameCycle) רשום צעד ביומן הצעדים
	if (steps.isNextStepOnIteration(gameCycle)) {
		// מוציאים את המקש מהתור ומבצעים אותו
		char key = steps.popStep();
		executeKeyAction(key);
	}
}


// Handles user input for player movement and game control
void GamePlay::handleInput() {
	if (_kbhit()) {
		char key = _getch();

		bool wasGameStep = executeKeyAction(key);

		if (isSaveMode && wasGameStep) {
			steps.addStep(gameCycle, key);
		}
	}
}


bool Game::executeKeyAction(char key) {
	if (key == ESC) {
		rooms[currentRoomIndex].printMessage("GAME PAUSED - ESC to resume | H for menu");
		key = _getch();
		if (key == 'H' || key == 'h') {
			inGame = false;
			return false;
		}

		else if (key == ESC) {
			rooms[currentRoomIndex].clearMessage();
			return false;
		}
	}

	bool effect = false;

	if (key == 'E' || key == 'e' || key == 'O' || key == 'o') {
		int playerIdx = (key == 'E' || key == 'e') ? 0 : 1;
		Player& p = players[playerIdx];
		Point pos = p.getPoint();

		if (p.hasAnyItem()) {
			switch (p.getItemType()) {

			case BOMB:
				rooms[currentRoomIndex].placeBombAt(pos, players[playerIdx].getSymbol());
				break;

			case TORCH:
				rooms[currentRoomIndex].placeTorchAt(pos);
				break;

			case KEY:
				rooms[currentRoomIndex].placeKeyAt(pos, p.getKeyPass());
				break;
			}
			p.removeItem();
			effect = true;
		}
	}
	else {
		for (auto& pl : players) {
			if (!pl.isAlive()) continue;
			pl.handleKeyPress(key);
			effect = true;
		}
	}
	return effect;
}


// Calculates the effective force of a player considering adjacent players pushing in the same direction
int Game::calcEffectiveForce(const Player& mover, Direction pushDir) const {
	const Player& other = (&mover == &players[0]) ? players[1] : players[0];

	int force = mover.getForce();

	int dx = std::abs(mover.getPoint().getX() - other.getPoint().getX());
	int dy = std::abs(mover.getPoint().getY() - other.getPoint().getY());
	bool adjacent = (dx + dy == 2);

	if (!adjacent)
		return force;

	if (other.isTryingToMove() && other.getMoveDir() == pushDir) {
		force += other.getForce();
	}

	return force;
}

// Checks if a player has met another player and handles the meeting
void Game::checkPlayerMeeting(Player* mover, MoveResult result) {
	if (result.type != MoveResultType::OtherPlayer) {
		launchMeetingActive = false;
		return;
	}

	if (launchMeetingActive)
		return;

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		Player& other = players[i];

		if (&other == mover)
			continue;

		if (launchMeetingActive)
			return;

		handleLaunchMeeting(*mover, other);
		launchMeetingActive = true;
		return;
	}
}

// Handles the meeting of two launched players
void Game::handleLaunchMeeting(Player& a, Player& b) {
	// Check launch status of both players
	bool aLaunched = a.isCurrentlyLaunched();
	bool bLaunched = b.isCurrentlyLaunched();

	// Neither launched
	if (!aLaunched && !bLaunched)
		return;

	// Both launched – cheek speed
	if (aLaunched && bLaunched) {
		int powerA = a.getLaunchSpeed();
		int powerB = b.getLaunchSpeed();

		if (powerA == powerB)
			return; // Equal power - no effect each other

		if (powerA > powerB) {// A affects B
			b.setCarriedBy(&a);
			justStartedCarry = true;
			return;
		}

		else {// B affects A
			a.setCarriedBy(&b);
			justStartedCarry = true;
			return;
		}
	}

	// Only A launched – affects B
	if (aLaunched) {
		b.setCarriedBy(&a);
		justStartedCarry = true;
		return;
	}

	// Only B launched – affects A
	else {
		a.setCarriedBy(&b);
		justStartedCarry = true;
		return;
	}

}

// Handles player death and respawn logic
void Game::handlePlayerDeath(Player& p) {
	p.loseLife();
	onPlayerDeath(p);
	drawLives();

	char symbol = p.getSymbol();

	if (!p.isAlive()) {
		p.stopLaunch();
		p.clearCarryLinks();

		gameOver(symbol);
		return;
	}
	else {
		std::string msg = "Player ";
		msg += symbol;
		msg += " lost a life!";

		rooms[currentRoomIndex].printMessage(msg.c_str());
		respawnPlayer(p);
	}
}

// If in save mode, records the player's death in the results log
void GamePlay::onPlayerDeath(Player& p) {
	if (isSaveMode) {
		results.addLifeLost(gameCycle, p.getId());
	}
}

void GameLoad::onPlayerDeath(Player& p) {
	const Results::Event* next = results.peek();

	if (next != nullptr && next->t == (size_t)gameCycle &&
		next->type == Results::Type::LifeLost && next->player == p.getId()) {

		results.popResult();
	}
	else {
		// במקום להדפיס ולהמשיך, בואי רק נדפיס פעם אחת ונעצור את הריצה
		static int lastErrorCycle = -1;
		if (lastErrorCycle != gameCycle) {
			std::cout << "Mismatch at cycle " << gameCycle << std::endl;
			lastErrorCycle = gameCycle;
			// inGame = false; // אפשר להוסיף את זה כדי לעצור את המשחק כשזה קורה
		}
	}

	// הקריאה הזו חייבת לקרות רק פעם אחת!
	Game::handlePlayerDeath(p);
}


// Respawns a player at their starting position based on their symbol
void Game::respawnPlayer(Player& p) {
	p.stopLaunch();
	p.clearCarryLinks();
	rooms[currentRoomIndex].clearCell(p.getPoint());

	if (p.getSymbol() == '$')
		p.setPoint(Point(5, 5, 1, 0, '$'));
	else
		p.setPoint(Point(55, 15, 1, 0, '&'));
}

// Draws the lives of both players on the HUD
void Game::drawLives() {
	printHearts(36, 1, players[0].getLives()); // PLAYER 1
	printHearts(75, 1, players[1].getLives()); // PLAYER 2
}

// Prints the number of hearts (lives) at a specified position
void Game::printHearts(int x, int y, int lives) {
	gotoxy(x, y);
	setTextColor(Color::RED);

	gotoxy(x, y);

	std::cout << lives << " ";

	setTextColor(Color::WHITE);
}
// Processes room events such as explosions and applies their effects to players
void Game::processRoomEvents() {
	Room& room = rooms[currentRoomIndex];
	auto events = room.consumeEvents();

	for (const auto& e : events) {
		if (e.type == GameEventType::Explosion) {
			applyExplosionToPlayers(e.pos, e.value, e.owner);
		}
	}
}

// Applies explosion effects to players within a certain radius, excluding the owner
void Game::applyExplosionToPlayers(const Point& center, int radius, char owner) {
	bool hit[NUM_PLAYERS] = { false, false };

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		Player& p = players[i];
		if (p.hasDisappeared()) continue;
		if (!p.isAlive()) continue;

		if (owner != '\0' && p.getSymbol() == owner)
			continue;

		const Point& pos = p.getPoint();

		int dx = pos.getX() - center.getX();
		int dy = pos.getY() - center.getY();

		if (std::abs(dx) <= radius && std::abs(dy) <= radius) {
			hit[i] = true;
		}
	}

	for (int i = 0; i < NUM_PLAYERS; ++i) {
		if (hit[i]) {
			handlePlayerDeath(players[i]);
		}
	}
}

// Checks if any player has a torch item
bool Game::anyPlayerHasTorch() const {
	return players[0].hasTorchItem() || players[1].hasTorchItem();
}

// Starts the dark room timer
void Game::startDarkTimer() {
	darkTimerActive = true;
	darkFramesLeft = (DARK_SECONDS * 1000 + FRAME_MS - 1) / FRAME_MS;
	darkLastShownSec = -1;
}

// Stops the dark room timer
void Game::stopDarkTimer() {
	darkTimerActive = false;
	darkFramesLeft = 0;
	darkLastShownSec = -1;
}

// Updates the dark room timer, handling player deaths if time runs out
void Game::updateDarkTimer() {
	Room& room = rooms[currentRoomIndex];
	int secLeft = (darkFramesLeft * FRAME_MS + 999) / 1000;

	if (!room.getIsDarkRoom()) {
		stopDarkTimer();
		room.clearDarkTimer();
		return;
	}
	bool hasTorch = anyPlayerHasTorch();

	if (hasTorch) {
		stopDarkTimer();
		room.clearDarkTimer();
		return;
	}

	if (!darkTimerActive) {
		startDarkTimer();
		room.printMessage("Dark room!");
		room.printDarkTimer(secLeft);
	}

	if (darkFramesLeft > 0)
		--darkFramesLeft;


	if (secLeft < 0) secLeft = 0;

	if (secLeft != darkLastShownSec) {
		room.printDarkTimer(secLeft);
		darkLastShownSec = secLeft;
	}

	if (darkFramesLeft <= 0) {
		for (int i = 0; i < NUM_PLAYERS; ++i) {
			if (players[i].isAlive() && !players[i].hasDisappeared()) {
				handlePlayerDeath(players[i]);
			}
		}

		startDarkTimer();
	}
}

// Checks if any other player is at a given point, excluding the specified player
bool Game::isPlayerAt(const Point& p, const Player* self) const {
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		const Player& other = players[i];
		if (&other != self && other.getPoint() == p && !other.hasDisappeared())
			return true;
	}
	return false;
}

// Displays the game over screen for a player who has lost all lives
void Game::gameOver(char loserSymbol) {
	cls();

	setTextColor(Color::RED);
	gotoxy(30, 10);
	std::cout << "GAME OVER";

	gotoxy(32, 12);
	std::cout << "O M G";

	setTextColor(Color::WHITE);
	gotoxy(26, 14);
	std::cout << "Player " << loserSymbol << " has been dead !!!!";

	gotoxy(32, 16);
	std::cout << "So sad...";

	gotoxy(28, 18);
	std::cout << "Be good next time!";
	gotoxy(26, 22);
	std::cout << "Press any key to restart";

	_getch();

	inGame = false;
	return;
}

// Displays the final screen with players' scores
void Game::showFinalScreen() {
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		players[i].setDisappeared(true);
		players[i].setRoom(nullptr);
	}
	FinalRoom::show(players[0].getScore(), players[1].getScore());
}

// Displays an invalid room file screen and returns true to go back to menu or false to exit
bool Game::showInvalidRoomScreen(const std::string& filename)
{
	cls();
	setTextColor(Color::WHITE);

	gotoxy(10, 8);
	std::cout << "INVALID / EMPTY ROOM FILE";

	gotoxy(10, 10);
	std::cout << "File: " << filename;

	gotoxy(10, 12);
	std::cout << "Press ANY KEY to return to menu.";
	gotoxy(10, 13);
	std::cout << "Press ESC (or Q) to exit.";

	int ch = _getch();
	if (ch == 27 || ch == 'q' || ch == 'Q') {
		return false;
	}
	return true;
}

// Gets the player ID based on the player reference
int Game::getPlayerId(const Player& p) const {
	for (int i = 0; i < NUM_PLAYERS; ++i) {
		if (&players[i] == &p) return i;
	}
	return -1;
}




