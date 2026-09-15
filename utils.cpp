#include "utils.h"

// Source: Course materials.

// Moves the console cursor to the specified (x, y) coordinates. 
// param x The column index (horizontal coordinate).
// param y The row index (vertical coordinate).
static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
static bool g_colorsEnabled = true;

void gotoxy(int x, int y) {
    std::cout.flush();
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

static void drawBox(int x, int y, int w, int h)
{
    char tl = '+', tr = '+', bl = '+', br = '+', hor = '-', ver = '|';

    gotoxy(x, y);
    std::cout << tl << std::string(w - 2, hor) << tr;

    for (int row = 1; row <= h - 2; ++row) {
        gotoxy(x, y + row);
        std::cout << ver << std::string(w - 2, ' ') << ver;
    }

    gotoxy(x, y + h - 1);
    std::cout << bl << std::string(w - 2, hor) << br;
}

// Hides the console cursor.
void hideCursor() {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &curInfo);
}

// Clears the console screen.
void cls() {
    system("cls");
}

// Enables or disables colored text output.
void setColorsEnabled(bool enabled) {
    g_colorsEnabled = enabled;
}

// Sets the text color for console output.
void setTextColor(Color color) {
    if (!g_colorsEnabled) {
        if (color != Color::DARK_GRAY) {
            color = Color::WHITE;
        }
    }

    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
}

static void printCenteredLine(int x, int y, int w, const std::string& text)
{
    int innerW = w - 2;
    int startX = x + 1 + ((std::max)(0, (innerW - (int)text.size()) / 2));
    gotoxy(startX, y);
    std::cout << text;
}

void popupMessageTimed(const std::string& title,
    const std::vector<std::string>& lines,
    int milliseconds,
    int screenW,
    int screenH)
{
    int w = 54;
    int h = 7 + (int)lines.size();

    w = (std::min)(w, screenW - 2);
    h = (std::min)(h, screenH - 2);

    int x = (screenW - w) / 2;
    int y = (screenH - h) / 2;

    drawBox(x, y, w, h);
    printCenteredLine(x, y + 1, w, title);

    int startY = y + 3;
    for (int i = 0; i < (int)lines.size(); ++i) {
        printCenteredLine(x, startY + i, w, lines[i]);
    }

    Sleep(milliseconds);
}


