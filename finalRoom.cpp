#include "finalRoom.h"
#include <iostream>
#include <string>
#include "utils.h"

// Map representation of the final room
static const char* finalMap[SCREEN_HEIGHT] = {
		"--------------------------------------------------------------------------------",
		"|                                                                              |",
		"|                                                                              |",
		"|------------------------------------------------------------------------------|",
		"|------------------------------------------------------------------------------|",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|                      C O N G R A T U L A T I O N S                           |",
		"|                                                                              |",
		"|                                $       &                                     |",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|                                                                              |",
		"|------------------------------------------------------------------------------|",
		"|------------------------------------------------------------------------------|",
		"|                                                                              |",
		"|                                                                              |",
		"--------------------------------------------------------------------------------"
};

// Helper function to print centered text
static void printCentered(int y, const std::string& text) {
	const int innerLeft = 1;
	const int innerWidth = SCREEN_WIDTH - 2;
	int x = innerLeft + (innerWidth - (int)text.size()) / 2;
	if (x < innerLeft) x = innerLeft;
	gotoxy(x, y);
	std::cout << text;
}

// Displays the final room with scores and winner information
void FinalRoom::show(int scoreP0, int scoreP1) {
	cls();
	setTextColor(Color::WHITE);

	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		gotoxy(0, y);
		std::cout << finalMap[y];
	}

	std::string centerLine, winnerLine;
	if (scoreP0 > scoreP1) { centerLine = "PLAYER $ WON THE GAME!"; winnerLine = "WINNER: PLAYER $"; }
	else if (scoreP1 > scoreP0) { centerLine = "PLAYER & WON THE GAME!"; winnerLine = "WINNER: PLAYER &"; }
	else { centerLine = "IT'S A TIE!"; winnerLine = "WINNER: NONE"; }

	printCentered(14, centerLine);
	printCentered(16, "PLAYER $ SCORE: " + std::to_string(scoreP0));
	printCentered(17, "PLAYER & SCORE: " + std::to_string(scoreP1));
	printCentered(18, winnerLine);

	setTextColor(Color::YELLOW); 
	printCentered(22, "Press any key to return to main menu...");

	_getch();
}


