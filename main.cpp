#include <iostream>
#include "Game.h"

/*
int main() {
	hideCursor();
	Game game;
	game.run();
}
*/

int main(int argc, char** argv) {
	hideCursor();

    bool isSave = false;
    bool isLoad = false;
    bool isSilent = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-save") isSave = true;
        else if (arg == "-load") isLoad = true;
        else if (arg == "-silent") isSilent = true;
    }

    Game* game = nullptr;

    if (isLoad) {
        game = new GameLoad(isSilent);
    }

    else {
        game = new GamePlay(isSave);
    }

    if (game) {
        game->run();
        delete game;
    }

    return 0;
}


