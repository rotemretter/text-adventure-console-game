#pragma once
#include <vector>
#include "Door.h"
#include "Point.h"

class DoorManager {

private:
    std::vector<Door> doors; //The list of doors in the current room.

public:
    // Door character range '1' to '9'.
    static constexpr char DOOR_MIN = '1';
    static constexpr char DOOR_MAX = '9';

    std::vector<Door>& getDoors() { return doors; }
    const std::vector<Door>& getDoors() const { return doors; }
    void addDoor(const Door& d);
    Door* getDoorAt(const Point& p);
    Door* getDoorByID(int id);
    static bool isDoorChar(char c);
    void clear();
};
