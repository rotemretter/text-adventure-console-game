# 2-Player Cooperative Text Adventure Engine

A feature-rich C++ console adventure game engine developed using Object-Oriented Programming (OOP) principles. The game features cooperative 2-player mechanics, room exploration via dynamic screen loading, puzzle solving, physical interaction systems, and deterministic record/replay testing modes.

## Key Features
- **Cooperative Multiplayer:** Dual-character keyboard control requiring players to cooperate to solve puzzles and advance through rooms.
- **Dynamic Physics & Mechanics:**
  - **Springs:** Compressible springs with directional launch acceleration and kinetic push behavior.
  - **Movable Obstacles:** Pushable blocks requiring individual or combined player force.
  - **Bombs & Destructibles:** Timed explosives destroying obstacles, walls, and entities within blast radius.
  - **World Interactions:** On/off switches, matching door keys, interactive riddles, and exit navigation.
- **File-Driven Level Architecture:** Dynamically parses room screens (`room1.screen`, `room2.screen`, `room3.screen`) and an external riddle bank (`riddles.txt`).
- **Deterministic Record & Replay System:**
  - Records gameplay steps cycle-by-cycle to `adv-world.steps`.
  - Validates gameplay events against `adv-world.result`.
  - Supports headless fast-forward testing (`-silent`) for automated regression validation.

## Controls

| Action | Player 1 | Player 2 |
| :--- | :---: | :---: |
| **Move Up** | `W` | `I` |
| **Move Down** | `X` | `M` |
| **Move Left** | `A` | `J` |
| **Move Right** | `D` | `L` |
| **Stay / Stop** | `S` | `K` |
| **Dispose Item** | `E` | `O` |

* **Pause Game:** Press `ESC` (resume playback or press `H` to return to the main menu).

## Project Structure
- **Core Engine:** `Game.cpp`/`h`, `main.cpp`, `utils.cpp`/`h`, `Direction.cpp`/`h`, `Point.cpp`/`h`
- **Entities & World:** `Player.cpp`/`h`, `Room.cpp`/`h`, `Door.h`, `DoorManager.cpp`/`h`, `finalRoom.cpp`/`h`, `Key.h`, `KeyManager.cpp`/`h`
- **Game Mechanics:** `Bomb.cpp`/`h`, `Obstacle.cpp`/`h`, `Spring.h`, `Switch.h`, `Riddle.h`, `RiddleBank.cpp`/`h`, `GameEvent.h`, `MoveResult.h`
- **Record & Replay:** `Steps.cpp`/`h`, `Results.cpp`/`h`, `adv-world.steps`, `adv-world.result`
- **Game Data:** `room1.screen`, `room2.screen`, `room3.screen`, `riddles.txt`

  ## Compilation & Execution

### Windows (MSVC)
Open Developer Command Prompt for Visual Studio and run:
`cl /EHsc /std:c++17 *.cpp /Fe:adv-world.exe`

### Windows (MinGW / GCC)
`g++ -std=c++17 *.cpp -o adv-world.exe`

### Running the Game
- **Standard interactive game:** `./adv-world.exe`
- **Play and record game steps:** `./adv-world.exe -save`
- **Replay recorded gameplay from file:** `./adv-world.exe -load`
- **Fast automated validation without console rendering:** `./adv-world.exe -load -silent`
