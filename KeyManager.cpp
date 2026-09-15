#include "KeyManager.h"
#include "Point.h"

// Adds a key to the manager.
void KeyManager::addKey(const Key& k) {
    keys.push_back(k);
}

// Gets the key located at a specific point.
Key* KeyManager::getKeyAt(const Point& p) {
    for (auto& key : keys) {
        const Point& loc = key.getLocation();
        if (loc.getX() == p.getX() && loc.getY() == p.getY())
        {
            return &key;
        }
    }
    return nullptr;
}

// Clears all keys from the manager.
void KeyManager::clear()
{
    keys.clear();
}




