#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <vector>

// Declarations only!
void gotoxy(int x, int y);
void hideCursor();
void cls();

// Constants for screen dimensions and game area boundaries
static const int SCREEN_WIDTH = 80;
static const int SCREEN_HEIGHT = 25;

static const int MIN_X = 0;
static const int MIN_Y = 0;

static const int GAME_AREA_LEFT = 1;
static const int GAME_AREA_RIGHT = 78;

static const int GAME_AREA_TOP = 5;
static const int GAME_AREA_BOTTOM = 18;

static const int ESC = 27;

enum class Color {
    BLACK = 0,
    DARK_BLUE = 1,
    DARK_GREEN = 2,
    DARK_CYAN = 3,
    DARK_RED = 4,
    DARK_MAGENTA = 5,
    DARK_YELLOW = 6,
    LIGHT_GRAY = 7,
    DARK_GRAY = 8,
    BLUE = 9,
    GREEN = 10,
    CYAN = 11,
    RED = 12,
    MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

void setTextColor(Color color);
void setColorsEnabled(bool enabled);
void drawBox(int x, int y, int w, int h);
void popupMessageTimed(const std::string& title, const std::vector<std::string>& lines, int milliseconds, int screenW = 80, int screenH = 25);
static void printCenteredLine(int x, int y, int w, const std::string& text);

#endif


