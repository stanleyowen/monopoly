#include "Game.h"
#include "GameConfig.h"

int main() {
	GameConfig::getInstance().loadConfig();

	Game game;
	game.start();
	return 0;
}
