#include "Card.h"
#include "Game/Map.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Tile.h"
#include "Game/GameConfig.h"
#include "MiniGame.h"
#include <limits>
#include <iostream>

Card::Card(const std::string &type) : type(type) {}

std::string Card::getType() const
{
	return type;
}

std::string Card::getAbbreviatedName() const
{
	GameConfig &config = GameConfig::getInstance();
	std::vector<CardConfig> cards = config.getCards();

	std::string t = type;

	// Trim leading/trailing spaces
	t.erase(0, t.find_first_not_of(" "));
	t.erase(t.find_last_not_of(" ") + 1);

	// Convert to lowercase for comparison
	std::string lower = t;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

	// Look for matching card in config to get its icon (abbreviation)
	for (const auto &card : cards)
	{
		std::string cardNameLower = card.name;
		std::transform(cardNameLower.begin(), cardNameLower.end(), cardNameLower.begin(), ::tolower);

		if (cardNameLower == lower)
		{
			// Use the icon from config as the abbreviation
			if (!card.icon.empty())
			{
				return card.icon;
			}
			break;
		}
	}

	// Fallback (first two non-space letters)
	std::string fallback;
	for (char c : t)
	{
		if (!isspace(c))
		{
			fallback += c;
		}
		if (fallback.size() == 2)
		{
			break;
		}
	}
	return fallback.empty() ? "??" : fallback;
}

void Card::applyEffect(Player &player, std::vector<Player> &players, Map &map)
{
	srand(static_cast<unsigned>(time(nullptr)));

	if (type == "Dice Card") // ����d
	{
		int diceValue;
		bool chose = 0;
		while (!chose)
		{
			std::cout << "Choose your dice value (2-12): ";
			std::cin >> diceValue;

			// If input fails (non-integer was entered)
			if (std::cin.fail())
			{
				// Clear the error flag
				std::cin.clear();
				// Discard invalid input
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}
			else
			{
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}

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

		if (std::cin.fail())
		{
			// Clear the error flag
			std::cin.clear();
			// Discard invalid input
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}
		else
		{
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}

		if (playerChoice <= 0 || playerChoice > players.size())
		{
			std::cout << "Invalid choice. Canceling destroy action.\n";
			return;
		}

		Player &targetPlayer = players[playerChoice - 1];
		const auto &properties = targetPlayer.getProperties();

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

			GameConfig &config = GameConfig::getInstance();
			auto locationMap = config.getLocationMap();

			// ����a�W
			std::string propertyName = (locationMap.find(tileId) != locationMap.end()) ? locationMap[tileId] : "Unknown";

			Tile &tile = map.getTile(x, y);
			int buildingLevel = tile.getPropertyLevel();

			std::cout << i + 1 << ". " << propertyName << " (Level " << buildingLevel << ")\n";
		}

		int propertyChoice;
		std::cout << "Enter property number (0 to cancel): ";
		std::cin >> propertyChoice;

		if (std::cin.fail())
		{
			// Clear the error flag
			std::cin.clear();
			// Discard invalid input
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}
		else
		{
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}

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
		Tile &tile = map.getTile(x, y);
		GameConfig &config = GameConfig::getInstance();
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
		std::cout << "Destroy Card used! Property \"" << propertyName << "\" has been destroyed.\n";
	}
	else if (type == "Fate Card")
	{ // �R�B�d
		std::cout << "Triggering a Fate event.\n";

		int roll = rand() % 100;

		if (roll < 25)
		{
			std::cout << "[Fate] A minigame has been triggered!\n\n";
			Utils::pressEnterToContinue();
			Utils::clearScreen();

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
			GameConfig &config = GameConfig::getInstance();
			auto valueRange = config.getEventValueRange();

			std::string typeUpper = "FATE";

			auto it = valueRange.find(typeUpper);
			if (it == valueRange.end())
			{
				std::cout << "[Error] No event value range defined for type: " << typeUpper << "\n";
				return;
			}

			// Determine if this will be a money or card effect
			int effectType = 0; // 0 = money, 1 = card

			if (effectType == 0)
			{
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
					"You received a mystery gift worth $"};
				std::vector<std::string> lossMsgs = {
					"You paid a fine of $",
					"You lost your wallet and lost $",
					"You bought a useless app for $"};

				int msgIndex = rand() % 3;
				std::string message = gain
										  ? gainMsgs[msgIndex] + std::to_string(actualAmount) + "."
										  : lossMsgs[msgIndex] + std::to_string(-actualAmount) + ".";

				std::cout << "[Fate] " << message << "\n";
			}
			else
			{
				// New card effect
				std::vector<CardConfig> cards = config.getCards();
				if (cards.empty())
				{
					std::cout << "[Fate] Nothing happened with cards.\n";
				}
				else
				{
					bool gain = rand() % 2;

					if (gain)
					{
						// Get a random card
						int cardIndex = rand() % cards.size();
						const CardConfig &selectedCard = cards[cardIndex];

						// Add the card to player's inventory
						Card card(selectedCard.name);
						player.addCard(card);

						std::cout << "[Fate] You found a " << selectedCard.name << " card!\n";
					}
					else if (!player.getCards().empty())
					{
						// Lose a random card if player has any
						int cardIndex = rand() % player.getCards().size();
						std::string cardName = player.getCards()[cardIndex].getType();

						// Remove the card
						player.removeCard(cardName);

						std::cout << "[Fate] Oh no! You lost your " << cardName << " card!\n";
					}
					else
					{
						std::cout << "[Fate] You had no cards to lose, lucky this time!\n";
					}
				}
			}
		}
		else
		{
			std::cout << "[Fate] But nothing happened...\n";
		}
		player.removeCard("Fate Card");
	}
	else if (type == "Rocket Card")
	{
		// Display all players that can be targeted
		std::cout << "Choose a player to send to the hospital:\n";
		std::vector<int> validChoices;

		for (size_t i = 0; i < players.size(); ++i)
		{
			// Don't allow targeting yourself
			if (&players[i] != &player)
			{
				std::cout << validChoices.size() + 1 << ". " << players[i].getName() << " (" << players[i].getSymbol() << ")\n";
				validChoices.push_back(i);
			}
		}

		// Get player choice
		int playerChoice = 0;
		while (playerChoice < 1 || playerChoice > validChoices.size())
		{
			std::cout << "Enter player number (1-" << validChoices.size() << "): ";
			std::cin >> playerChoice;
			std::cin.ignore();

			if (std::cin.fail() || playerChoice < 1 || playerChoice > validChoices.size())
			{
				std::cin.clear();
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

				std::cout << "Invalid choice. Please try again.\n";
				playerChoice = 0;
			}
		}

		// Select the target player
		Player &targetPlayer = players[validChoices[playerChoice - 1]];

		// Find hospital tile location on the map
		int hospitalX = -1;
		int hospitalY = -1;

		// Find the hospital tile by scanning the map
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				Tile &tile = map.getTile(x, y);
				if (tile.getSymbol() == 'H') // Hospital is marked with 'H'
				{
					hospitalX = x;
					hospitalY = y;
					break;
				}
			}
			if (hospitalX != -1)
				break;
		}

		// Move the player to the hospital if found
		if (hospitalX != -1 && hospitalY != -1)
		{
			// Send to hospital for 2 turns
			targetPlayer.sendToHospital(2);

			// Move player to hospital location
			targetPlayer.setPosition(hospitalX, hospitalY);
			std::cout << targetPlayer.getName() << " was sent to the hospital for 2 rounds!\n";

			// Remove the used card
			player.removeCard("Rocket Card");
		}
		else
		{
			std::cout << "Error: Hospital tile not found on the map!\n";
		}
	}
	else
	{
		std::cout << "Unknown or unsupported card type.\n";
	}
}
