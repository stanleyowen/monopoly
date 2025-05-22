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
#include <fstream>
#include <nlohmann/json.hpp>

void Game::setGameState(GameState state)
{
	currentState = state;
}

GameState Game::getGameState() const
{
	return currentState;
}

Game Game::instance;

Game &Game::getInstance()
{
	return instance;
}

Map &Game::getMap()
{
	return map;
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

Game::Game() : currentPlayerIndex(0), gameRunning(true) {}

void Game::initializePlayers()
{
	if (!players.empty())
	{
		players.clear();
	}

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

		players.back().addCard(Card("Dice Card")); // 骰控卡
		// players.back().addCard(Card("Barrier Card"));   // 路障卡
		players.back().addCard(Card("Destroy Card")); // 拆除卡
		players.back().addCard(Card("Rocket Card"));  // 火箭卡
		players.back().addCard(Card("Fate Card"));	  // 命運卡
	}
}

std::vector<Player> &Game::getPlayers()
{
	return players;
}

int Game::getCurrentPlayerIndex() const
{
	return currentPlayerIndex;
}

void Game::setCurrentPlayerIndex(int index)
{
	if (index >= 0 && index < players.size())
	{
		currentPlayerIndex = index;
	}
	else
	{
		std::cerr << "Invalid player index: " << index << std::endl;
	}
}

int Game::getTileIdByName(const std::string &name) const
{
	const auto &config = GameConfig::getInstance();
	const auto &tiles = config.getBoardTiles();

	for (const auto &tile : tiles)
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
	currentState = GameState::INIT;

	while (gameRunning)
	{
		switch (currentState)
		{
		case GameState::INIT:
			Utils::clearScreen();
			initializePlayers();
			map.setupBoard();
			map.drawBoard(players);
			currentState = GameState::START;
			break;

		case GameState::START:
			processTurn();
			break;

		case GameState::MOVED:
			handleTileEvents(players[currentPlayerIndex]);
			currentState = GameState::ROUND_END;
			break;

		case GameState::ROUND_END:
			checkWinCondition();
			if (!gameRunning)
				currentState = GameState::FINISH;
			else
			{
				currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
				currentState = GameState::START;
			}
			break;

		case GameState::FINISH:
			gameRunning = false;
			displayGameOver();
			break;
		}
	}
}

void Game::animateControlledPlayerMovement(Player &player, int steps, int diceValue)
{
	// Get the animation settings from the configuration
	const auto &config = GameConfig::getInstance();
	bool isAnimationEnabled = config.getAnimation();
	int animationSpeed = config.getAnimationTime(); // Retrieve animation speed (in milliseconds)

	// 無動畫情況，直接移動
	if (!isAnimationEnabled)
	{
		player.move(steps);
		map.drawBoard(players);
		std::cout << "\n[Dice Control] You directly moved " << steps << " steps.\n";
		std::cout << "Dice result: (" << diceValue << ")\n";

		if (diceValue >= 1 && diceValue <= 6)
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

		// Pause for the duration specified by the animation speed
		std::this_thread::sleep_for(std::chrono::milliseconds(animationSpeed));
	}

	std::cout << "[Completed] You moved " << steps << " steps using Dice Control.\n";
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
	srand(static_cast<unsigned>(time(nullptr)));

	if (players.empty())
	{
		std::cerr << "No players available!\n";
		currentState = GameState::FINISH;
		return;
	}

	Player &currentPlayer = players[currentPlayerIndex];

	// Hospital logic
	if (currentPlayer.isInHospital())
	{
		bool hospitalActionDone = false;
		while (!hospitalActionDone && currentPlayer.isInHospital())
		{
			std::string input;

			std::cout << "[Hospital] " << currentPlayer.getName() << " is in the hospital!\n";
			std::cout << "Turns left: " << currentPlayer.getHospitalTurnsLeft() << "\n\n";

			Utils::displayDialogue("player_action.moved.hospital");
			std::cout << "> ";
			std::getline(std::cin, input);

			if (!input.empty() && input[0] == '/')
			{
				Command command;
				command.execute(*this, input);
			}
			else if (input == "T" || input == "t")
			{
				int dice1 = rand() % 6 + 1;
				int dice2 = rand() % 6 + 1;
				int diceRoll = dice1 + dice2;
				std::cout << "[Hospital] You rolled (" << dice1 << ", " << dice2 << ") = " << diceRoll << "\n";

				if (diceRoll >= 10)
				{
					std::cout << "[Hospital] You rolled 10 or more! You are released from the hospital!\n";
					currentPlayer.leaveHospital();
				}
				else
				{
					std::cout << "[Hospital] Not enough! You remain in the hospital.\n";
					currentPlayer.decrementHospitalTurns();
				}

				hospitalActionDone = true;
			}
			else if (input == "R" || input == "r")
			{
				int bill = 2000;

				if (currentPlayer.getMoney() >= bill)
				{
					currentPlayer.addMoney(-bill);
					std::cout << "[Hospital] You paid $" << bill << " and are released from the hospital!\n";
					currentPlayer.leaveHospital();
				}
				else
				{
					std::cout << "[Hospital] Not enough money to pay the bill!\n";
				}

				hospitalActionDone = true;
			}
			else if (input == "I" || input == "i")
			{
				currentPlayer.showInfo();
				std::cout << "Press Enter to return to hospital menu...";
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}
			else
			{
				currentPlayer.decrementHospitalTurns();
				std::cout << "[Hospital] You skipped your chance to roll. Turns left: " << currentPlayer.getHospitalTurnsLeft() << "\n";
				hospitalActionDone = true;
			}
		}

		Utils::pressEnterToContinue();
		Utils::clearScreen();
		map.drawBoard(players);

		if (currentPlayer.isInHospital())
		{
			// End turn if still in hospital
			currentState = GameState::ROUND_END;
			return;
		}
		else
		{
			std::cout << "[Hospital] You are free to move next turn!\n";
			currentState = GameState::ROUND_END;
			return;
		}
	}

	// Player input loop
	while (true)
	{
		std::cout << "\nIt's " << currentPlayer.getSymbol() << " " << currentPlayer.getName() << "'s turn:\n\n";
		Utils::displayDialogue("player_action.start");
		std::string input;
		std::cout << "> ";
		std::getline(std::cin, input);

		if (input == "T" || input == "t")
		{
			int dice1, dice2, diceRoll{};

			if (currentPlayer.hasNextDiceValue())
			{
				int totalValue = currentPlayer.getNextDiceValue();

				// Split the total value into two dice (1-6 each)
				// For values 2-7, use a random split
				// For values 8-12, ensure each die is at most 6
				if (totalValue <= 7)
				{
					dice1 = rand() % (totalValue - 1) + 1; // Random value between 1 and totalValue-1
					dice2 = totalValue - dice1;
				}
				else
				{
					dice1 = std::min(6, totalValue - 1);
					dice2 = totalValue - dice1;
				}

				diceRoll = totalValue;
				currentPlayer.clearNextDiceValue();
			}
			else
			{
				dice1 = rand() % 6 + 1;
				dice2 = rand() % 6 + 1;
				diceRoll = dice1 + dice2;
			}

			int currentPositionId = currentPlayer.getPositionId();
			int boardSize = 28;

			// Check if player will pass or land on Start after moving
			if (diceRoll > 0)
			{
				int newPositionId = (currentPositionId + diceRoll) % boardSize;
				bool passingStart = (currentPositionId + diceRoll) >= boardSize || (newPositionId < currentPositionId && newPositionId != 0);

				if (passingStart)
				{
					currentPlayer.addMoney(GameConfig::getInstance().getPassingStartBonus());
					std::cout << "[系統] " << currentPlayer.getName() << " 通過起點，獲得 $" << GameConfig::getInstance().getPassingStartBonus() << "!\n";
				}
			}

			// Now display animation and move the player
			if (currentPlayer.hasNextDiceValue())
			{
				animateControlledPlayerMovement(currentPlayer, diceRoll, dice1);
			}
			else
			{
				displayDiceAnimation(dice1, dice2, players);
				animatePlayerMovement(currentPlayer, diceRoll, dice1, dice2);
			}

			currentState = GameState::MOVED;
			break;
		}
		else if (input == "I" || input == "i")
		{
			currentPlayer.showInfo();

			int cardChoice;
			std::cout << "Enter the card number to use (0 to cancel): ";
			std::cin >> cardChoice;

			// If input fails (non-integer was entered)
			if (std::cin.fail())
			{
				// Clear the error flag
				std::cin.clear();
				// Discard invalid input
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				std::cout << "Invalid input. Please enter a number.\n";
			}
			else
			{
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}

			if (cardChoice > 0 && cardChoice <= currentPlayer.getCards().size())
			{
				Card chosenCard = currentPlayer.getCards()[cardChoice - 1];
				chosenCard.applyEffect(currentPlayer, players, map);
			}
			else if (cardChoice == 0)
			{
				std::cout << "Returning to game.\n";
			}
			else
			{
				std::cout << "Invalid choice. Returning to game.\n";
			}
		}
		else if (!input.empty() && input[0] == '/')
		{
			Command command;
			command.execute(*this, input);
			// Check if game was ended during command
			if (!gameRunning)
				currentState = GameState::FINISH;
			return;
		}
		else
		{
			Utils::displayDialogue("invalid_input");
		}

		Utils::pressEnterToContinue();
		Utils::clearScreen();
		map.drawBoard(players);
	}
}

// Function to display dice rolling animation for two dice
void Game::displayDiceAnimation(int dice1, int dice2, const std::vector<Player> &players)
{
	srand(static_cast<unsigned>(time(nullptr)));

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

void Game::handleTileEvents(Player &player)
{
	Tile &currentTile = map.getTile(player.getX(), player.getY());
	currentTile.handleEvent(player, map);

	Utils::clearScreen();
	map.drawBoard(players);
}

void Game::checkWinCondition()
{
	int winMoney = GameConfig::getInstance().getWinMoney();

	if (winMoney <= 0)
	{
		winMoney = 300000;
		std::cout << "[Warning] Invalid winMoney in config. Defaulting to $300000.\n";
	}

	// Check if any player meets the win condition
	for (const auto &p : players)
	{
		if (p.getMoney() >= winMoney)
		{
			gameRunning = false;
			break;
		}
	}

	// Check survival (only one left with money)
	int aliveCount = 0;
	for (const auto &p : players)
		if (p.getMoney() > 0)
			++aliveCount;

	if (aliveCount == 1 || !gameRunning)
	{
		gameRunning = false;
		Utils::clearScreen();
		displayGameOver();
	}
}

void Game::displayGameOver()
{
	Utils::clearScreen();

	std::cout << R"(
  ______                                            ______                                 __ 
 /      \                                          /      \                               /  |
/$$$$$$  |  ______   _____  ____    ______        /$$$$$$  | __     __  ______    ______  $$ |
$$ | _$$/  /      \ /     \/    \  /      \       $$ |  $$ |/  \   /  |/      \  /      \ $$ |
$$ |/    | $$$$$$  |$$$$$$ $$$$  |/$$$$$$  |      $$ |  $$ |$$  \ /$$//$$$$$$  |/$$$$$$  |$$ |
$$ |$$$$ | /    $$ |$$ | $$ | $$ |$$    $$ |      $$ |  $$ | $$  /$$/ $$    $$ |$$ |  $$/ $$/ 
$$ \__$$ |/$$$$$$$ |$$ | $$ | $$ |$$$$$$$$/       $$ \__$$ |  $$ $$/  $$$$$$$$/ $$ |       __ 
$$    $$/ $$    $$ |$$ | $$ | $$ |$$       |      $$    $$/    $$$/   $$       |$$ |      /  |
 $$$$$$/   $$$$$$$/ $$/  $$/  $$/  $$$$$$$/        $$$$$$/      $/     $$$$$$$/ $$/       $$/ 
                                                                                              
)" << "\n";

	// Sort players by money descending
	std::vector<Player> sortedPlayers = players;
	std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](const Player &a, const Player &b)
			  { return a.getMoney() > b.getMoney(); });

	int count = (sortedPlayers.size() < 3) ? sortedPlayers.size() : 3;
	const std::vector<std::string> places = {"[1] 1st Place", "[2] 2nd Place", "[3] 3rd Place"};

	for (int i = 0; i < count; ++i)
	{
		const Player &p = sortedPlayers[i];
		std::cout << places[i] << ": " << p.getSymbol() << " " << p.getName() << " | $" << p.getMoney() << "\n";

		// Cards
		std::cout << "  Cards: ";
		const auto &cards = p.getCards();
		if (cards.empty())
			std::cout << "None";
		else
		{
			for (const auto &card : cards)
				std::cout << card.getAbbreviatedName() << " ";
		}
		std::cout << "\n";

		// Properties
		std::cout << "  Properties: ";
		const auto &props = p.getProperties();
		if (props.empty())
			std::cout << "None";
		else
		{
			for (const auto &pos : props)
				std::cout << "(" << pos.first << "," << pos.second << ") ";
		}
		std::cout << "\n\n";
	}

	std::cout << "Congratulations! Thanks for playing.\n";
}

bool Game::saveGame(const std::string &filename)
{
	nlohmann::json saveData;

	// Save players
	for (const auto &player : players)
	{
		nlohmann::json p;
		p["name"] = player.getName();
		p["symbol"] = player.getSymbol();
		p["color"] = player.getColor();
		p["money"] = player.getMoney();
		p["x"] = player.getX();
		p["y"] = player.getY();

		// Cards as vector of strings
		std::vector<std::string> cardTypes;
		for (const auto &card : player.getCards())
			cardTypes.push_back(card.getType());
		p["cards"] = cardTypes;

		// Properties as vector of {x, y}
		nlohmann::json props = nlohmann::json::array();
		for (const auto &prop : player.getProperties())
			props.push_back({{"x", prop.first}, {"y", prop.second}});
		p["properties"] = props;

		p["status"] = {
			{"inHospital", player.isInHospital()},
			{"hospitalTurnsLeft", player.getHospitalTurnsLeft()}};
		saveData["players"].push_back(p);
	}

	// Save only the tiles on the board perimeter (the outer ring)
	saveData["tiles"] = nlohmann::json::array();
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			// Only save tiles on the perimeter
			if (i == 0 || i == 7 || j == 0 || j == 7)
			{
				Tile &tile = map.getTile(i, j);
				nlohmann::json t;
				t["x"] = i;
				t["y"] = j;
				t["symbol"] = std::string(1, tile.getSymbol());
				t["owner"] = tile.getOwner();
				t["level"] = tile.getPropertyLevel();
				t["isOccupied"] = tile.getOccupied();
				saveData["tiles"].push_back(t);
			}
		}
	}

	saveData["currentPlayerIndex"] = currentPlayerIndex;

	std::ofstream out(filename);
	if (!out.is_open())
		return false;
	out << saveData.dump(4);
	return true;
}

bool Game::loadGame(const std::string &filename)
{
	std::ifstream in(filename);
	if (!in.is_open())
		return false;

	nlohmann::json saveData;
	in >> saveData;

	players.clear();
	for (const auto &p : saveData["players"])
	{
		Player player(p["name"], p["symbol"], p["money"]);
		player.setColor(p["color"]);
		player.setPosition(p["x"], p["y"]);

		// Cards
		for (const auto &cardType : p["cards"])
			player.addCard(Card(cardType));

		// Properties
		for (const auto &prop : p["properties"])
			player.addProperty(prop["x"], prop["y"]);

		player.setInHospital(p["status"]["inHospital"]);
		player.setHospitalTurnsLeft(p["status"]["hospitalTurnsLeft"]);
		players.push_back(player);
	}

	// Setup the board first
	map.setupBoard();

	// Then load saved tile data
	for (const auto &t : saveData["tiles"])
	{
		int x = t["x"];
		int y = t["y"];
		Tile &tile = map.getTile(x, y);

		// If tile contains symbol, set it
		if (t.contains("symbol"))
		{
			char symbol = t["symbol"].get<std::string>()[0];
			tile.setSymbol(symbol);
		}

		tile.setOwner(t["owner"]);
		tile.setPropertyLevel(t["level"]);
		tile.setOccupied(t["isOccupied"]);
	}

	currentPlayerIndex = saveData["currentPlayerIndex"];
	return true;
}