#pragma once
#include "Point.h"

class Door {

private:
    int id;               // Door number 1-9.
    bool requiresKey;    // Does the door need a key to open?
    bool isOpen;        // Check if the door open.
    Point position;    // Door location on the map
    int targetRoom;   // The next room.
    std::vector<int> requiredSwitches; // Switches that need to be activated to open the door.

public:

    // Default constructor
    Door(int id, Point pos, bool needsKey, Point keyPos = Point(), int target = -1)
        : id(id),
        position(pos),
        requiresKey(needsKey),
        isOpen(false),
        targetRoom(target) {
    }

    int getID() const { return id; }
    const Point& getPosition() const { return position; }
    int getTargetRoom() const { return targetRoom; }
    bool reqKey() const { return requiresKey; }
    bool open() const { return isOpen; }
    void setOpen(bool val) { isOpen = val; }

    // Add a required switch to the door
    void addRequiredSwitch(int sId) {
        requiredSwitches.push_back(sId);
    }

    // Get the list of required switches
    const std::vector<int>& getRequiredSwitches() const {
        return requiredSwitches;
    }
};

















