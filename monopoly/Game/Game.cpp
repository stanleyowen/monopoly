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
#include <limits>
#include <ios>

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
	)" };

Game::Game() : currentPlayerIndex(0), gameRunning(true)
{
	initializePlayers();
}

void Game::initializePlayers()
{
	const auto& config = GameConfig::getInstance();
	const auto& playerNames = config.getPlayerNames();
	const auto& playerIcons = config.getPlayerIcons();
	const auto& playerColors = config.getPlayerColors();
	int startMoney = config.getStartMoney();

	for (size_t i = 0; i < playerNames.size(); ++i)
	{
		std::string symbol = (i < playerIcons.size()) ? playerIcons[i] : "?";
		std::string color = (i < playerColors.size()) ? playerColors[i] : "\033[0m"; // Default to no color
		players.emplace_back(playerNames[i], symbol, startMoney);
		players.back().setColor(color);
	}
}

std::vector<Player>& Game::getPlayers()
{
	return players;
}

int Game::getCurrentPlayerIndex() const
{
	return currentPlayerIndex;
}

int Game::getTileIdByName(const std::string& name) const
{
	const auto& config = GameConfig::getInstance();
	const auto& tiles = config.getBoardTiles();

	for (const auto& tile : tiles)
	{
		if (tile.name == name)
		{
			return tile.id;
		}
	}
	return -1; // 地名不存在
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

void Game::animateControlledPlayerMovement(Player& player, int steps, int diceValue)
{
	// Get the animation settings from the configuration
	const auto& config = GameConfig::getInstance();
	bool isAnimationEnabled = config.getAnimation();
	int animationSpeed = config.getAnimationTime(); // Retrieve animation speed (in milliseconds)

	// 無動畫情況，直接移動
	if (!isAnimationEnabled)
	{
		player.move(steps);
		map.drawBoard(players);
		std::cout << "\n[Dice Control] You directly moved " << steps << " steps.\n";
		std::cout << "Dice result: (" << diceValue << ")\n";
		std::cout << diceFaces[diceValue - 1] << std::endl;
		return;
	}

	// 使用動畫的情況
	for (int i = 0; i < steps; ++i)
	{
		// Move the player one step
		player.move(1);

		// Clear the screen
		Utils::clearScreen();

		// Redraw the board with the updated player position
		map.drawBoard(players);

		// 顯示骰控卡的單骰效果
		std::cout << "\n[Dice Control] Moving with controlled dice value: " << diceValue << "\n";
		std::cout << "Step " << (i + 1) << " / " << steps << "\n";
		std::cout << "Dice result: (" << diceValue << ")\n";
		//std::cout << diceFaces[diceValue - 1] << std::endl;

		// Pause for the duration specified by the animation speed
		std::this_thread::sleep_for(std::chrono::milliseconds(animationSpeed));
	}

	std::cout << "[Completed] You moved " << steps << " steps using Dice Control.\n";
}

void Game::animatePlayerMovement(Player& player, int steps, int dice1, int dice2)
{
	// Get the animation settings from the configuration
	const auto& config = GameConfig::getInstance();
	bool isAnimationEnabled = config.getAnimation();
	int animationSpeed = config.getAnimationTime(); // Retrieve animation speed (in milliseconds)

	// Check if animation is disabled
	if (!isAnimationEnabled)
	{
		// If animation is disabled, move the player directly
		player.move(steps);
		map.drawBoard(players);
		std::cout << "\nDice roll result:\n";
		std::cout << "(" << dice1 << ", " << dice2 << ") -> Move forward " << steps << " steps.\n";
		std::cout << diceFaces[dice1 - 1] << "    " << diceFaces[dice2 - 1] << std::endl;
		return;
	}

	// If animation is enabled, perform the animation
	for (int i = 0; i < steps; ++i)
	{
		// Move the player one step
		player.move(1);

		// Clear the screen
		Utils::clearScreen();

		// Redraw the board with the updated player position
		map.drawBoard(players);

		// Display the dice result below the board
		std::cout << "\nDice Result:\n";
		std::cout << "(" << dice1 << ", " << dice2 << ") -> Move forward " << steps << " steps.\n";
		std::cout << diceFaces[dice1 - 1] << "    " << diceFaces[dice2 - 1] << std::endl;

		// Pause for the duration specified by the animation speed
		std::this_thread::sleep_for(std::chrono::milliseconds(animationSpeed));
	}
}

void Game::processTurn()
{
	if (players.empty())
	{
		std::cerr << "No players available!\n";
		return;
	}

	Player& currentPlayer = players[currentPlayerIndex];

	// Display dialogue for the current player's turn
	std::cout << "\nIt's " << currentPlayer.getSymbol() << " " << currentPlayer.getName() << "'s turn:\n\n";
	Utils::displayDialogue("player_action.start");
	std::string input;
	std::cout << "> ";
	std::getline(std::cin, input);

	if (input == "T" || input == "t")
	{
		// Seed the random number generator
		std::srand(static_cast<unsigned>(std::time(nullptr)));

		int dice1, dice2, diceRoll;

		// 檢查是否有骰控卡效果
		if (currentPlayer.hasNextDiceValue())
		{
			// 使用骰控卡效果，直接控制骰子值
			dice1 = currentPlayer.getNextDiceValue();
			dice2 = dice1;  // 單骰效果，設為相同值
			diceRoll = dice1;
			currentPlayer.clearNextDiceValue();
			std::cout << "Using Dice Control: You rolled " << dice1 << ".\n";
			animateControlledPlayerMovement(currentPlayer, diceRoll, dice1);
		}
		else
		{
			// 正常擲骰邏輯
			std::srand(static_cast<unsigned>(std::time(nullptr)));
			dice1 = rand() % 6 + 1;
			dice2 = rand() % 6 + 1;
			diceRoll = dice1 + dice2;

			// 正常骰子動畫
			displayDiceAnimation(dice1, dice2, players);
			animatePlayerMovement(currentPlayer, diceRoll, dice1, dice2);

		}
		handleTileEvents(currentPlayer);
		checkWinCondition();
	}
	else if (input == "I" || input == "i")
	{
		currentPlayer.showInfo();

		// 提示玩家選擇卡片編號
		int cardChoice;
		std::cout << "Enter the card number to use (0 to cancel): ";
		std::cin >> cardChoice;

		// 若選擇有效卡片
		if (cardChoice > 0 && cardChoice <= currentPlayer.getCards().size())
		{
			Card chosenCard = currentPlayer.getCards()[cardChoice - 1];
			// 呼叫卡片效果，傳入玩家及所有玩家列表
			chosenCard.applyEffect(currentPlayer, this->getPlayers());
		}
		else {
			std::cout << "Invalid choice. Returning to game.\n";
		}
	}
	else if (!input.empty() && input[0] == '/')
	{
		Command command;
		command.execute(*this, input);

		return; // 不更換玩家回合
	}
	else
	{
		Utils::displayDialogue("invalid_input");
	}

	currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
}

// Function to display dice rolling animation for two dice
void Game::displayDiceAnimation(int dice1, int dice2, const std::vector<Player>& players)
{
	// Get the animation settings from the configuration
	const auto& config = GameConfig::getInstance();
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
			Utils::clearScreen();

			// Draw the map on top
			map.drawBoard(players);

			// Display the two dice side by side
			std::cout << diceFaces[randomFace1] << "    " << diceFaces[randomFace2] << std::endl;

			// Pause for the duration specified by the animation speed
			std::this_thread::sleep_for(std::chrono::milliseconds(animationSpeed));
		}
	}

	// Clear the console and display the final result
	Utils::clearScreen();
}

void Game::handleTileEvents(Player& player)
{
	Tile& currentTile = map.getTile(player.getX(), player.getY());
	currentTile.handleEvent(player);

	Utils::clearScreen();
	map.drawBoard(players);
}

void Game::checkWinCondition()
{
	int winMoney = GameConfig::getInstance().getWinMoney();

	// Fallback default if config file failed to load properly
	if (winMoney <= 0) {
		winMoney = 300000;
		std::cout << "[Warning] Invalid winMoney in config. Defaulting to $300000.\n";
	}

	int aliveCount = 0;
	int richestIndex = -1;

	for (int i = 0; i < players.size(); ++i)
	{
		if (players[i].getMoney() >= winMoney)
		{
			std::cout << "🏆 " << players[i].getSymbol() << " " << players[i].getName()
				<< " wins with $" << players[i].getMoney() << "!\n";
			gameRunning = false;
			return;
		}

		if (players[i].getMoney() > 0)
		{
			aliveCount++;
			richestIndex = i;
		}
	}

	// Only one player still has money
	if (aliveCount == 1 && richestIndex != -1)
	{
		std::cout << "🏆 " << players[richestIndex].getSymbol() << " " << players[richestIndex].getName()
			<< " wins by survival with $" << players[richestIndex].getMoney() << "!\n";
		gameRunning = false;
	}
}
