#pragma once
#include <vector>
#include "Key.h"
#include "Point.h"

class KeyManager {

private:
    std::vector<Key> keys; //The list of keys in the current room.

public:
    static constexpr char KEY_CHAR = 'K';

    void addKey(const Key& k);      // Adds a key to the manager.
    Key* getKeyAt(const Point& p); // Gets the key located at a specific point.
    void clear();                 // Clears all keys from the manager.

    static bool isKeyChar(char c) { return c == KEY_CHAR; }// Checks if a character represents a key.
};






