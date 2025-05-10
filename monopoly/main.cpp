#include "Game.h"
#include "GameConfig.h"
#include "WelcomingScreen/WelcomingScreen.h"

int main()
{
	WelcomingScreen welcomeScreen;
	welcomeScreen.displayWelcomeScreen();
	welcomeScreen.displayStartGame();

	// Load game configuration
	GameConfig::getInstance().loadConfig();

	// Start the game
	Game game;
	game.start();

	return 0;
}