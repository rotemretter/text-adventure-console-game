#ifndef POINT_H
#define POINT_H

#include "utils.h"
#include <iostream>
#include "Direction.h"

// The Point class stores coordinates(x, y), movement deltas, and a character.
class Point {
private:
    int x, y;            // Current position on screen
    int diff_x, diff_y; // Movement deltas
    char ch;           // Character displayed at this point

public:
    //Default constructor initializes point at (0,0)
    Point()
        : x(0),
        y(0),
        diff_x(0),
        diff_y(0),
        ch(' ') {
    }

    // Creates a point at (x1, y1) with movement (diffx, diffy) and a display character c.
    Point(int x1, int y1, int diffx, int diffy, char c)
        : x(x1),
        y(y1),
        diff_x(diffx),
        diff_y(diffy),
        ch(c) {
    }

    Point(int px, int py)
        : x(px),
        y(py),
        diff_x(0),
        diff_y(0),
        ch(' ') {
    }

    // Moves the point by its movement deltas.
    void moveInDirection(Direction dir) {
        switch (dir) {
        case Direction::UP:    y--; break;
        case Direction::DOWN:  y++; break;
        case Direction::LEFT:  x--; break;
        case Direction::RIGHT: x++; break;
        }
    }

    // Equality operator to compare two points based on their coordinates.
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    // Draws the point using its stored character
    void draw() const {
        draw(ch);
    }

    void draw(char c) const {
        gotoxy(x, y);
        std::cout << c;
    }


    void move();
    void setDirection(Direction dir);
    void setChar(char c) { ch = c; }
    char getChar() const { return ch; }


    int getX() const { return x; }
    int getY() const { return y; }

    int setX(int newX) { return x = newX; }
    int setY(int newY) { return y = newY; }

    Direction getDirection() const;
    int getDiffX() const { return diff_x; }
    int getDiffY() const { return diff_y; }

};

#endif

















