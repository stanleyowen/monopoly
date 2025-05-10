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
	int startMoney = config.getStartMoney();

	for (size_t i = 0; i < playerNames.size(); ++i)
	{
		std::string symbol = (i < playerIcons.size()) ? playerIcons[i] : "?";
		players.emplace_back(playerNames[i], symbol, startMoney);
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
		int diceRoll = rollDice();
		currentPlayer.move(diceRoll);

		handleTileEvents(currentPlayer);
		std::cout << "Rolled: " << diceRoll << std::endl
				  << std::endl;
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

// Function to display dice rolling animation for two dice
void displayDiceAnimation(int dice1, int dice2, const std::vector<Player> &players)
{
	// Show rolling animation for a fixed duration
	for (int i = 0; i < 10; ++i)
	{
		int randomFace1 = rand() % 6; // Random face for dice 1
		int randomFace2 = rand() % 6; // Random face for dice 2

		// Clear the console
		std::cout << "\033[2J\033[H";

		// Draw the map on top
		map.drawBoard(players);

		// Display the two dice side by side
		std::cout << diceFaces[randomFace1] << "    " << diceFaces[randomFace2] << std::endl;

		// Pause for 200ms
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	// Clear the console and display the final result
	std::cout << "\033[2J\033[H";
	std::cout << "Final Dice Roll:\n";
	std::cout << diceFaces[dice1 - 1] << "    " << diceFaces[dice2 - 1] << std::endl;

	std::cout << "Total: " << (dice1 + dice2) << std::endl;
	std::cout << "Press Enter to continue...\n";
	std::cin.get();
}

// Function to roll two dice and return the sum
int Game::rollDice()
{
	srand(time(0));

	int dice1 = rand() % 6 + 1;
	int dice2 = rand() % 6 + 1;

	// Display the animation and final result
	displayDiceAnimation(dice1, dice2, players);

	return dice1 + dice2; // Return the sum of the two dice
}

void Game::handleTileEvents(Player &player)
{
	Tile &currentTile = map.getTile(player.getX(), player.getY());
	currentTile.handleEvent(player);

	clearScreen();
	map.drawBoard(players);
	map.getTile(player.getX(), player.getY()).handleEvent(player);
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
