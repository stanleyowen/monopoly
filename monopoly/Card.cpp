#include "Card.h"
#include "Game/Map.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Tile.h"
#include "Game/GameConfig.h"
#include "MiniGame.h"
#include <iostream>

Card::Card(const std::string& type) : type(type) {}

std::string Card::getType() const
{
	return type;
}

std::string Card::getAbbreviatedName() const
{
	std::string t = type;

	// Trim leading/trailing spaces
	t.erase(0, t.find_first_not_of(" "));
	t.erase(t.find_last_not_of(" ") + 1);

	// Convert to lowercase for comparison
	std::string lower = t;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

	// Match normalized names
	if (lower == "dice card")
		return "Di";
	if (lower == "destroy card")
		return "Dst";
	if (lower == "fate card")
		return "Fa";
	if (lower == "rocket card")
		return "Ro";
	if (lower == "barrier card")
		return "Ba";

	// Fallback (first two non-space letters)
	std::string fallback;
	for (char c : t)
	{
		if (!isspace(c))
			fallback += c;
		if (fallback.size() == 2)
			break;
	}
	return fallback.empty() ? "??" : fallback;
}

void Card::applyEffect(Player& player, std::vector<Player>& players, Map& map)
{
	if (type == "Dice Card") // ����d
	{
		int diceValue;
		bool chose = 0;
		while (!chose)
		{
			std::cout << "Choose your dice value (2-12): ";
			std::cin >> diceValue;
			std::cin.ignore();

			if (diceValue >= 2 && diceValue <= 12)
			{
				chose = 1;
				player.setNextDiceValue(diceValue);
				std::cout << "Dice Control Card used! Next dice will roll: " << diceValue << "\n";
				player.removeCard("Dice Card");
			}
			else
			{
				std::cout << "Invalid dice value. Please enter a number between 2 and 12.\n";
			}
		}
	}
	else if (type == "Barrier Card")
	{ // ���٥d
		std::cout << "Placing a barrier on a tile to block players.\n";
		// Implement barrier logic here
	}
	else if (type == "Destroy Card")
	{ // ��d

		std::cout << "Choose a player to destroy their property:\n";

		// �C�X�Ҧ����a
		for (size_t i = 0; i < players.size(); ++i)
		{
			std::cout << i + 1 << ". " << players[i].getName() << "\n";
		}

		int playerChoice;
		std::cout << "Enter player number (0 to cancel): ";
		std::cin >> playerChoice;
		std::cin.ignore();

		if (playerChoice <= 0 || playerChoice > players.size())
		{
			std::cout << "Invalid choice. Canceling destroy action.\n";
			return;
		}

		Player& targetPlayer = players[playerChoice - 1];
		const auto& properties = targetPlayer.getProperties();

		// �ˬd���a�O�_���в�
		if (properties.empty())
		{
			std::cout << targetPlayer.getName() << " has no properties to destroy.\n";
			return;
		}

		// �C�X�ؼЪ��a���Ҧ��в�
		std::cout << "Choose a property to destroy:\n";
		for (size_t i = 0; i < properties.size(); ++i)
		{
			// ���T����в���m (x, y)
			int x = properties[i].first;
			int y = properties[i].second;

			// �p�� tileId (���]�a�Ϭ� 8x8)
			int tileId = -1;

			if (x == 0)
				tileId = y;
			else if (y == 7)
				tileId = 7 + x;
			else if (x == 7)
				tileId = 7 + 7 + (7 - y);
			else if (y == 0)
				tileId = 7 + 7 + 7 + (7 - x);

			GameConfig& config = GameConfig::getInstance();
			auto locationMap = config.getLocationMap();

			// ����a�W
			std::string propertyName = (locationMap.find(tileId) != locationMap.end()) ? locationMap[tileId] : "Unknown";

			Tile& tile = map.getTile(x, y);
			int buildingLevel = tile.getPropertyLevel();

			std::cout << i + 1 << ". " << propertyName << " (Level " << buildingLevel << ")\n";
		}

		int propertyChoice;
		std::cout << "Enter property number (0 to cancel): ";
		std::cin >> propertyChoice;
		std::cin.ignore();

		if (propertyChoice <= 0 || propertyChoice > properties.size())
		{
			std::cout << "Invalid choice. Canceling destroy action.\n";
			return;
		}

		// ����襤���в��y��
		int x = properties[propertyChoice - 1].first;
		int y = properties[propertyChoice - 1].second;

		// �p�� tileId
		int tileId = -1;

		if (x == 0)
			tileId = y;
		else if (y == 7)
			tileId = 7 + x;
		else if (x == 7)
			tileId = 7 + 7 + (7 - y);
		else if (y == 0)
			tileId = 7 + 7 + 7 + (7 - x);

		// Game& game = Game::getInstance();
		// Map& map = game.getMap();
		Tile& tile = map.getTile(x, y);
		GameConfig& config = GameConfig::getInstance();
		auto locationMap = config.getLocationMap();
		std::string propertyName = (locationMap.find(tileId) != locationMap.end()) ? locationMap[tileId] : "Unknown";

		if (tile.getPropertyLevel() > 1)
		{
			tile.setPropertyLevel(tile.getPropertyLevel() - 1);
			std::cout << "Property \"" << propertyName << "\" downgraded to Level " << tile.getPropertyLevel() << "!\n";
		}
		else if (tile.getPropertyLevel() == 1)
		{
			tile.setPropertyLevel(tile.getPropertyLevel() - 1);
			tile.setOccupied(false);
			tile.setOwner("");
			targetPlayer.removeProperty(x, y);

			std::cout << "Property \"" << propertyName << "\" destroyed!\n";
		}
		player.removeCard("Destroy Card");

		// Clear the screen and redraw the board
		Utils::pressEnterToContinue();
		Utils::clearScreen();
		map.drawBoard(players);
		std::cout << "Destroy Card used! Property \"" << propertyName << "\" has been destroyed.\n";
	}
	else if (type == "Fate Card")
	{ // �R�B�d
		std::cout << "Triggering a Fate event.\n";

		srand(static_cast<unsigned>(time(nullptr)));
		int roll = rand() % 100;

		if (roll < 25)
		{
			std::cout << "[Fate] A minigame has been triggered!\n\n";
			int gameType = rand() % 4;
			if (gameType == 0)
				MiniGame::playHorseRace(player);
			else if (gameType == 1)
				MiniGame::playDragonGate(player);
			else if (gameType == 2)
				MiniGame::playMazeEscape(player);
			else
				MiniGame::playTreasureHunt(player);
		}
		else if (roll < 95) // 25-94 → 70% chance
		{
			GameConfig& config = GameConfig::getInstance();
			auto valueRange = config.getEventValueRange();

			std::string typeUpper = "FATE";

			auto it = valueRange.find(typeUpper);
			if (it == valueRange.end())
			{
				std::cout << "[Error] No event value range defined for type: " << typeUpper << "\n";
				return;
			}

			int min = it->second.first;
			int max = it->second.second;
			int amount = rand() % (max - min + 1) + min;
			bool gain = rand() % 2;
			int actualAmount = gain ? amount : -amount;

			if (actualAmount > 0)
				player.addMoney(actualAmount);
			else
				player.subtractMoney(-actualAmount);

			std::vector<std::string> gainMsgs = {
				"You found $",
				"You sold old stuff and earned $",
				"You received a mystery gift worth $" };
			std::vector<std::string> lossMsgs = {
				"You paid a fine of $",
				"You lost your wallet and lost $",
				"You bought a useless app for $" };

			int msgIndex = rand() % 3;
			std::string message = gain
				? gainMsgs[msgIndex] + std::to_string(actualAmount) + "."
				: lossMsgs[msgIndex] + std::to_string(-actualAmount) + ".";

			std::cout << "[Fate] " << message << "\n";
		}
		else
		{
			std::cout << "[Fate] But nothing happened...\n";
		}
		player.removeCard("Fate Card");
	}
	else if (type == "Rocket Card")
	{ // ���b�d
		std::cout << "Sending a player to the hospital for 2 turns.\n";

		// List all players except the current player
		std::cout << "Choose a player to send to the hospital (cannot choose yourself):\n";
		int index = 1;
		std::vector<int> validChoices;

		for (size_t i = 0; i < players.size(); ++i)
		{
			if (players[i].getName() != player.getName()) // �L�o���ۤv
			{
				std::cout << index << ". " << players[i].getName() << "\n";
				validChoices.push_back(i); // �s�U���Ŀﶵ����گ���
				index++;
			}
		}

		int playerChoice;
		std::cout << "Enter player number (0 to cancel): ";
		std::cin >> playerChoice;
		std::cin.ignore();

		if (playerChoice <= 0 || playerChoice > validChoices.size())
		{
			std::cout << "Invalid choice. Canceling rocket action.\n";
			return;
		}

		// �����ܪ����a
		Player& targetPlayer = players[validChoices[playerChoice - 1]];

		targetPlayer.sendToHospital();
		std::cout << targetPlayer.getName() << " was sent to the hospital for 2 rounds!\n";
		map.drawBoard(players);
		player.removeCard("Rocket Card");
	}
	else
	{
		std::cout << "Unknown or unsupported card type.\n";
	}
}
