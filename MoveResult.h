#pragma once

// Represents the result of a player's move attempt.
enum class MoveResultType { None, Door, Riddle, Obstacle, Blocked, OtherPlayer };

struct MoveResult {
    MoveResultType type = MoveResultType::None;
    int roomId = -1;
};




