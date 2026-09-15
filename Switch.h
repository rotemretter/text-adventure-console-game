#pragma once
#include "Point.h"

class Switch {

private:
    int id;
    Point location; // Switch location in the room.
    bool isOn;     // Switch status: on or off.

public:
    static constexpr char SWITCH_ON_CHAR = '/';
    static constexpr char SWITCH_OF_CHAR = '\\';

    // Default constructor
    Switch(int ID, Point p, bool switchOn = false)
        : id(ID),
        location(p),
        isOn(switchOn)
    {
    }

    const Point& getLocation() const { return location; }
    int getId() const { return id; }

    // Change the switch status.
    void change() {
        isOn = !isOn;
    }

    // Get the current status of the switch.
    bool getStatus() const {
        return isOn;
    }

    // Check if a character represents a switch.
    static bool isSwitchChar(char c) {
        return c == SWITCH_ON_CHAR || c == SWITCH_OF_CHAR;
    }

    // Draw the switch at its location.
    void draw() {
        location.draw(isOn ? SWITCH_ON_CHAR : SWITCH_OF_CHAR);
    }

    // Get the character representing the switch's current state.
    char getChar() const {
        return isOn ? SWITCH_ON_CHAR : SWITCH_OF_CHAR;
    }

	// Check if the switch is activated (on).
    bool isActivated() const {
        return isOn;
    }
};




#pragma once




