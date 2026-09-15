#pragma once
#include "Point.h"

class Key {

private:
    Point keyLocation;  // Location of the key on the map.
    char ch;		   // Character representing the key.
    int password;     // Passwor associated with the key that opens specific doors.

public:
    // Default constructor
    Key()
        : keyLocation(Point(-1, -1, 0, 0, ' ')),
        ch('K'),
        password(0) {
    }

    Key(const Point& p, char c, int pass)
        : keyLocation(p),
        ch(c),
        password(pass) {
    }

    int getPass() const { return password; }
    char getChar() const { return ch; }
    const Point& getLocation() const { return keyLocation; }

    void setLocation(const Point& p) { keyLocation = p; }

    void draw() { keyLocation.draw(ch); }     // Draws the key at its location.
    void erase() { keyLocation.draw(' '); }  // Erases the key from its location.

    // Deletes the key from the map.
    void deleteKey() {
        erase();
        keyLocation = Point(-1, -1, 0, 0, ' '); // Invalidate the key location.
    }

    // Drops the key at a new location and draws it.
    void dropKeyAt(const Point& p) {
        keyLocation = p;
        p.draw();
    }
};









