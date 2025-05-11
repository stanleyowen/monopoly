#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "GameConfig.h"
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

const std::vector<std::string> diceFaces = {
	R"(
	 +-------+
	 |       |
	 |   o   |
	 |       |
	 +-------+
	)",
	R"(
	 +-------+
	 | o     |
	 |       |
	 |     o |
	 +-------+
	)",
	R"(
	 +-------+
	 | o     |
	 |   o   |
	 |     o |
	 +-------+
	)",
	R"(
	 +-------+
	 | o   o |
	 |       |
	 | o   o |
	 +-------+
	)",
	R"(
	 +-------+
	 | o   o |
	 |   o   |
	 | o   o |
	 +-------+
	)",
	R"(
	 +-------+
	 | o   o |
	 | o   o |
	 | o   o |
	 +-------+
	)"};

Game::Game() : currentPlayerIndex(0), gameRunning(true)
{
	initializePlayers();
}

void clearScreen()
{
	std::system("cls||clear"); // Use system command to clear the screen
}

void Game::initializePlayers()
{
	const auto &config = GameConfig::getInstance();
	const auto &playerNames = config.getPlayerNames();
	const auto &playerIcons = config.getPlayerIcons();
	const auto &playerColors = config.getPlayerColors();
	int startMoney = config.getStartMoney();

	for (size_t i = 0; i < playerNames.size(); ++i)
	{
		std::string symbol = (i < playerIcons.size()) ? playerIcons[i] : "?";
		std::string color = (i < playerColors.size()) ? playerColors[i] : "\033[0m"; // Default to no color
		players.emplace_back(playerNames[i], symbol, startMoney);
		players.back().setColor(color);
	}
}

std::vector<Player> &Game::getPlayers()
{
	return players;
}

void Game::start()
{
	map.setupBoard();
	map.drawBoard(players);

	while (gameRunning)
	{
		processTurn();
	}

	std::cout << "\nGame Over!" << std::endl;
}

void Game::animatePlayerMovement(Player &player, int steps, int dice1, int dice2)
{
	// Get the animation settings from the configuration
	const auto &config = GameConfig::getInstance();
	bool isAnimationEnabled = config.getAnimation();
	int animationSpeed = config.getAnimationTime(); // Retrieve animation speed (in milliseconds)

	// Check if animation is disabled
	if (!isAnimationEnabled)
	{
		// If animation is disabled, move the player directly
		player.move(steps);
		map.drawBoard(players);
		std::cout << "\nDice Result:\n";
		std::cout << diceFaces[dice1 - 1] << "    " << diceFaces[dice2 - 1] << std::endl;
		return;
	}

	// If animation is enabled, perform the animation
	for (int i = 0; i < steps; ++i)
	{
		// Move the player one step
		player.move(1);

		// Clear the screen
		clearScreen();

		// Redraw the board with the updated player position
		map.drawBoard(players);

		// Display the dice result below the board
		std::cout << "\nDice Result:\n";
		std::cout << diceFaces[dice1 - 1] << "    " << diceFaces[dice2 - 1] << std::endl;

		// Pause for the duration specified by the animation speed
		std::this_thread::sleep_for(std::chrono::milliseconds(animationSpeed));
	}
}

void Game::processTurn()
{
	// Check that there are players
	if (players.empty())
	{
		std::cerr << "No players available!\n";
		return;
	}

	Player &currentPlayer = players[currentPlayerIndex];

	std::cout << currentPlayer.getName() << "'s turn!\n\n";
	std::cout << "Please choose one of the following actions:\n";
	std::cout << "T: Roll the dice\n";
	std::cout << "I: View player info\n";
	std::cout << "(Or type /list for all available commands)\n";

	std::string input;
	std::cout << "> ";

	if (std::cin.peek() == '\n')
		std::cin.ignore(); // ignore leftover newline
	std::getline(std::cin, input);

	if (input == "T" || input == "t")
	{
		// Seed the random number generator
		std::srand(static_cast<unsigned>(std::time(nullptr)));

		// Roll the dice and get the result
		int dice1 = rand() % 6 + 1;
		int dice2 = rand() % 6 + 1;

		// Display the dice animation
		displayDiceAnimation(dice1, dice2, players);

		// Calculate the total dice roll
		int diceRoll = dice1 + dice2;

		// Animate the player's movement
		animatePlayerMovement(currentPlayer, diceRoll, dice1, dice2);

		handleTileEvents(currentPlayer);

		checkWinCondition();
	}
	else if (input == "I" || input == "i")
	{
		currentPlayer.showInfo();
	}
	else if (!input.empty() && input[0] == '/')
	{
		Command command;
		command.execute(*this, input);
	}
	else
	{
		std::cout << "Invalid choice. Please select 'T', 'I', or a valid slash command.\n\n";
	}

	// Move to the next player
	currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
}

// Function to display dice rolling animation for two dice
void Game::displayDiceAnimation(int dice1, int dice2, const std::vector<Player> &players)
{
	// Get the animation settings from the configuration
	const auto &config = GameConfig::getInstance();
	bool isAnimationEnabled = config.getAnimation();
	int animationSpeed = config.getAnimationTime();

	// If animation is enabled, perform the dice rolling animation
	if (isAnimationEnabled)
	{
		for (int i = 0; i < 10; ++i)
		{
			int randomFace1 = rand() % 6; // Random face for dice 1
			int randomFace2 = rand() % 6; // Random face for dice 2

			// Clear the console
			std::system("cls||clear");

			// Draw the map on top
			map.drawBoard(players);

			// Display the two dice side by side
			std::cout << diceFaces[randomFace1] << "    " << diceFaces[randomFace2] << std::endl;

			// Pause for the duration specified by the animation speed
			std::this_thread::sleep_for(std::chrono::milliseconds(animationSpeed));
		}
	}

	// Clear the console and display the final result
	std::system("cls||clear");
	map.drawBoard(players);
	std::cout << "Final Dice Roll:\n";
	std::cout << diceFaces[dice1 - 1] << "    " << diceFaces[dice2 - 1] << std::endl;
	std::cout << "Total: " << (dice1 + dice2) << std::endl;
}

void Game::handleTileEvents(Player &player)
{
	Tile &currentTile = map.getTile(player.getX(), player.getY());
	currentTile.handleEvent(player);

	clearScreen();
	map.drawBoard(players);
}

void Game::checkWinCondition()
{
	int winMoney = GameConfig::getInstance().getWinMoney();
	if (winMoney == 0)
		winMoney = 300000;
	int aliveCount = 0;
	int richestIndex = -1;

	for (int i = 0; i < players.size(); ++i)
	{
		if (players[i].getMoney() >= winMoney)
		{
			std::cout << "🏆 " << players[i].getName() << " wins with $" << players[i].getMoney() << "!\n";
			gameRunning = false;
			return;
		}
		if (players[i].getMoney() > 0)
		{
			aliveCount++;
			richestIndex = i;
		}
	}

	if (aliveCount == 1)
	{
		std::cout << "🏆 " << players[richestIndex].getName() << " wins by survival!\n";
		gameRunning = false;
	}
}
