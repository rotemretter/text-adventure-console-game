#include "DoorManager.h"
#include "Point.h"

// DoorManager class methods implementation.
//-----------------------------------------------//

// Adds a door to the manager.
void DoorManager::addDoor(const Door& d) {
    doors.push_back(d);
}

// Gets the door located at a specific point.
Door* DoorManager::getDoorAt(const Point& p) {
    for (auto& d : doors)
        if (d.getPosition() == p)
            return &d;
    return nullptr;
}

// Gets the door with a specific ID.
Door* DoorManager::getDoorByID(int id) {
    for (auto& d : doors)
        if (d.getID() == id)
            return &d;
    return nullptr;
}

// Clears all doors from the manager.
void DoorManager::clear() {
    doors.clear();
}

// Checks if a character represents a door.
bool DoorManager::isDoorChar(char c) {
    return (c >= DOOR_MIN && c <= DOOR_MAX);
}


















