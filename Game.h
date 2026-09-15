#pragma once
#include "Player.h"
#include "Room.h"
#include "Riddle.h"
#include "RiddleBank.h"
#include <vector>
#include <string>
#include <filesystem>
#include "Steps.h"
#include "Results.h"

using std::vector;

// Manages the entire flow of the game, including the main menu,
// starting and running the game loop, handling input, and updating players
class Game {
public:
	enum class MenuChoice : char {
		Start = '1',
		Instructions = '8',
		Exit = '9'
	};

	static constexpr int FIRST_ROOM_ID = 1;
	static constexpr int FIRST_ROOM_INDEX = 0;

	static constexpr int NUM_PLAYERS = 2;

	static constexpr int FRAME_MS = 80;
	static constexpr int DARK_SECONDS = 15;

	static constexpr int SLEEP_TIME_PLAY = 80;

protected:
	bool isRunning;                       // App running flag.
	bool inGame;                          // In-game session flag.

	std::vector<Room> rooms;              // Loaded rooms.
	std::vector<std::string> roomFiles;   // Room filenames.

	int currentRoomIndex;                 // Current room index.

	Player players[NUM_PLAYERS];          // Players.

	RiddleBank riddleBank;                // Riddles manager.

	bool launchMeetingActive = false;     // Meeting handling guard.
	bool justStartedCarry = false;        // First-tick carry/push flag.

	int score = 0;                        // (Likely unused) Global score.

	bool lastDrawDark = false;            // Last dark draw mode.

	bool darkTimerActive = false;         // Dark timer active.
	int  darkFramesLeft = 0;             // Dark timer frames left.
	int  darkLastShownSec = -1;         // Last shown seconds.

	int lastLivesShown[NUM_PLAYERS] = { -1, -1 };                    // HUD cache: lives.
	int lastScoreShown[NUM_PLAYERS] = { -1, -1 };                   // HUD cache: score.
	ItemType lastItemShown[NUM_PLAYERS] = { NONE_ITEM, NONE_ITEM };// HUD cache: item.
	bool hudInited = false;                                       // HUD initialized.

	static constexpr int NO_ROOM_REQUEST = -1;     // No room request.
	static constexpr int FINAL_ROOM_REQUEST = -2; // Final-screen request.

	bool isLoadMode = false;     // If started with -load 
	bool isSaveMode = false;    // If started with -save
	bool isSilentMode = false; // If started with -silent

	Steps steps;          // Steps object 
	Results results;     // Results object

	size_t gameCycle = 0;      // Current game cycle count
	long randomSeed = 0;      // The random seed used for the game


	// Virtual methods for input handling and player death
	virtual void handleInput() = 0;
	virtual void onPlayerDeath(Player& p) = 0;
	virtual int getSleepTime() const = 0;
	virtual bool shouldDraw() const { return true; }
	virtual void initSeed() = 0;
	virtual void handleEndOfGame() {}

public:
	Game(bool save = false, bool load = false, bool silent = false);
	virtual ~Game() {}

	virtual void run() = 0;
	void mainMenu();
	void gameLoop();
	void updateAndRedraw();
	bool executeKeyAction(char key);
	void startNewGame();
	void showInstructions();
	void drawStatusBar();
	void invalidateHud();
	void moveToRoom(int roomID, int triggeringPlayerId);
	void initPlayers();
	void initInRoom();
	void setupPlayers(bool isNewGame);
	bool showRiddleFullScreen(const Riddle& r, std::string& userAnswer);
	void redrawCurrentRoom();
	void runRiddleFlow(Player& p, Room& room, const Point& oldPlayerPos);
	void handleColor(char cell);
	void checkPlayerMeeting(Player* mover, MoveResult result);
	void handleLaunchMeeting(Player& a, Player& b);
	int calcEffectiveForce(const Player& mover, Direction pushDir) const;
	void handlePlayerDeath(Player& p);
	void respawnPlayer(Player& p);
	void updateLightingForCurrentRoom();
	void drawLives();
	void printHearts(int x, int y, int lives);
	void processRoomEvents();
	void applyExplosionToPlayers(const Point& center, int radius, char owner);
	bool anyPlayerHasTorch() const;
	void startDarkTimer();
	void stopDarkTimer();
	void updateDarkTimer();
	static std::vector<std::string> getAllRoomFileNames();
	void gameOver(char loserSymbol);
	bool isPlayerAt(const Point& p, const Player* self) const;
	void showFinalScreen();
	bool showInvalidRoomScreen(const std::string& filename);
	int getPlayerId(const Player& p) const;

	// Access to justStartedCarry flag
	bool isJustStartedCarry() const {
		return justStartedCarry;
	}
};


class GamePlay : public Game {
public:
	GamePlay(bool save);

protected:
	void handleInput() override;
	void onPlayerDeath(Player& p) override;
	int getSleepTime() const override { return SLEEP_TIME_PLAY; }
	void initSeed() override;
	void handleEndOfGame() override;
	void run() override;
};


class GameLoad : public Game {
private:
	bool isSilent;

public:
	GameLoad(bool silent);

protected:
	void handleInput() override;
	void onPlayerDeath(Player& p) override;
	int getSleepTime() const override { return isSilent ? 0 : 20; }
	bool shouldDraw() const override { return !isSilent; }
	void initSeed() override;
	void run() override;
};




