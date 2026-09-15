#pragma once
#include "Point.h"

// Represents different types of game events.
enum class GameEventType {
    Explosion
};

struct GameEvent {
    GameEventType type;
    Point pos;
    int value = 0;
    char owner = '\0';
};



