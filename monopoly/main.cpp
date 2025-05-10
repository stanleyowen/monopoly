#include "Game/Game.h"
#include "Game/GameConfig.h"
#include "WelcomingScreen/WelcomingScreen.h"

int main()
{
	// Display the welcoming screen for the first 3 seconds
	// WARNING: This part is commented out in development mode to increase efficiency
	// WelcomingScreen welcomeScreen;
	// welcomeScreen.displayWelcomeScreen();
	// welcomeScreen.displayStartGame();

	// Load game configuration
	GameConfig::getInstance().loadConfig();

	// Start the game
	Game game;
	game.start();

	return 0;
}