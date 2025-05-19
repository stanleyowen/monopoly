#include "Utils.h"
#include "Game/Game.h"
#include "Game/GameConfig.h"
#include "WelcomingScreen/WelcomingScreen.h"

int main()
{
	// Load game configuration
	GameConfig::getInstance().loadConfig();
	Utils::loadDialogueData();

	// Display the welcoming screen for the first 3 seconds
	// WARNING: This part is commented out in development mode to increase efficiency
	WelcomingScreen welcomeScreen;
	welcomeScreen.displayWelcomeScreen();
	welcomeScreen.displayStartGame();

	// Start the game
	Game& game = Game::getInstance();
	game.start();

	return 0;
}