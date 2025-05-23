#include "Tile.h"
#include "Utils.h"
#include "Game/Player.h"
#include "Game/GameConfig.h"
#include "Game/Game.h"
#include "MiniGame.h"
#include <iostream>
#include <algorithm>

Tile::Tile() : symbol(' '), isOccupied(false) {}

Tile::Tile(char symbol) : symbol(symbol), isOccupied(false) {}

char Tile::getSymbol() const
{
	return symbol;
}

void Tile::setSymbol(char newSymbol)
{
	symbol = newSymbol;
}

bool Tile::getOccupied() const
{

	return isOccupied;
}

void Tile::setOccupied(bool occupied)
{
	isOccupied = occupied;
}

void Tile::setOwner(const std::string &newOwner)
{
	owner = newOwner;
}

std::string Tile::getOwner() const
{
	return owner;
}

int Tile::getPropertyLevel() const
{
	return propertyLevel;
}

void Tile::setPropertyLevel(int level)
{
	if (level >= 1 && level <= 3)
	{
		propertyLevel = level;
	}
	else
	{
		std::cerr << "Invalid property level. Must be between 1 and 3.\n";
	}
}

int Tile::getPositionId() const
{
	return positionId;
}

void Tile::handleEvent(Player &player, Map &map)
{
	GameConfig &config = GameConfig::getInstance();
	Game &game = Game::getInstance();
	srand(static_cast<unsigned>(time(nullptr)));

	auto boardTiles = config.getBoardTiles();

	// Get the player's current position as an index
	int index = player.getPositionId();

	// Find the tile configuration based on the player's index
	auto it = std::find_if(boardTiles.begin(), boardTiles.end(), [index](const TileConfig &tile)
						   {
							   return tile.id == index; // Match the index directly
						   });

	if (it == boardTiles.end())
	{
		std::cout << "Invalid tile configuration for index " << index << ".\n";
		return;
	}

	const TileConfig &tileConfig = *it;

	std::cout << player.getName() << " landed on " << tileConfig.name << "!\n";

	if (tileConfig.type == "start")
	{
		std::cout << "You received a bonus of $" << config.getPassingStartBonus() << "!\n";
	}
	else if (tileConfig.type == "store")
	{
		bool chose = 0;
		while (!chose)
		{
			Utils::displayDialogue("player_action.moved.store");

			char choice;
			std::cout << "> ";
			std::cin >> choice;

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

			if (choice == 'E' || choice == 'e')
			{
				enterShop(game, player);
				chose = 1;
			}
			else if (choice == 'I' || choice == 'i')
			{
				player.showInfo();

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
					cardChoice = 0; // Set a default value to cancel
				}
				else
				{
					std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				}

				if (cardChoice > 0 && cardChoice <= player.getCards().size())
				{
					Card chosenCard = player.getCards()[cardChoice - 1];
					std::vector<Player> &players = game.getPlayers();
					chosenCard.applyEffect(player, players, map);
				}
				else
				{
					std::cout << "Returning to game.\n";
				}
			}
			else
			{
				chose = 1;
				std::cout << "You chose to pass.\n";
			}
		}

		Utils::pressEnterToContinue();
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (tileConfig.type == "fate")
	{
		std::cout << "You triggered a Fate tile...\n\n";

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
		else if (roll < 95) // 25–94 → 70% chance
		{
			GameConfig &config = GameConfig::getInstance();
			auto valueRange = config.getEventValueRange();

			std::string typeUpper = tileConfig.type;
			std::transform(typeUpper.begin(), typeUpper.end(), typeUpper.begin(), ::toupper);

			auto it = valueRange.find(typeUpper);
			if (it == valueRange.end())
			{
				std::cout << "[Error] No event value range defined for type: " << typeUpper << "\n";
				return;
			}

			// Determine if this will be a money or card effect
			int effectType = rand() % 2; // 0 = money, 1 = card

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

				std::cout << "[Fate] " << message << "\n\n";
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
			std::cout << "[Fate] But nothing happened...\n\n";
		}

		Utils::pressEnterToContinue();
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (tileConfig.type == "chance")
	{
		std::cout << "You triggered a Chance tile...\n\n";

		int roll = rand() % 100;

		if (roll < 35)
		{
			std::cout << "[Chance] A minigame has been triggered!\n\n";
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
		else if (roll < 90) // 35–89 → 55% chance
		{
			GameConfig &config = GameConfig::getInstance();
			auto valueRange = config.getEventValueRange();

			std::string typeUpper = tileConfig.type;
			std::transform(typeUpper.begin(), typeUpper.end(), typeUpper.begin(), ::toupper);

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
				"You won the lottery and gained $",
				"You sold a startup and received $",
				"You got a lucky inheritance of $",
				"Your stocks exploded! You gained $"};
			std::vector<std::string> lossMsgs = {
				"You got scammed and lost $",
				"You paid unexpected hospital bills of $",
				"Your car broke down. Repairs cost $",
				"You invested in crypto at the wrong time. Lost $"};

			int msgIndex = rand() % 4;
			std::string message = gain
									  ? gainMsgs[msgIndex] + std::to_string(actualAmount) + "!"
									  : lossMsgs[msgIndex] + std::to_string(-actualAmount) + "...";

			std::cout << "[Chance] " << message << "\n\n";
		}
		else
		{
			std::cout << "[Chance] But nothing happened...\n\n";
		}

		Utils::pressEnterToContinue();
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (tileConfig.type == "hospital")
	{
		std::cout << player.getName() << " has been sent to the hospital and must skip 3 turns (unless rolling 10+ to leave)!\n";
		player.sendToHospital();
		Utils::pressEnterToContinue();
		return;
	}
	else if (tileConfig.type == "property")
	{
		if (!getOccupied())
		{

			bool chose = 0;
			while (!chose)
			{
				char choice;
				std::cout << tileConfig.id << " " << tileConfig.name << " Price:" << tileConfig.price << "\n";
				Utils::displayDialogue("player_action.moved.property_unowned");
				std::cout << "> ";
				std::cin >> choice;

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

				if (choice == 'R' || choice == 'r')
				{
					chose = 1;
					if (player.getMoney() >= tileConfig.price)
					{
						player.subtractMoney(tileConfig.price);
						player.addProperty(player.getX(), player.getY());
						setOwner(player.getName());
						setOccupied(true);
						std::cout << "You have purchased this property.\n";
					}
					else
					{
						std::cout << "You don't have enough money to buy this property.\n";
					}
				}
				else if (choice == 'I' || choice == 'i')
				{
					player.showInfo();

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
						cardChoice = 0; // Set a default value to cancel
					}
					else
					{
						std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
					}

					if (cardChoice > 0 && cardChoice <= player.getCards().size())
					{
						Card chosenCard = player.getCards()[cardChoice - 1];
						std::vector<Player> &players = game.getPlayers();
						chosenCard.applyEffect(player, players, map);
					}
					else
					{
						std::cout << "Returning to game.\n";
					}
				}
				else
				{
					chose = 1;
					std::cout << "You chose not to buy the property.\n";
				}

				Utils::pressEnterToContinue();
				Utils::clearScreen();
				game.getMap().drawBoard(game.getPlayers());
			}
		}
		else if (getOccupied() && getOwner() != player.getName())
		{
			int totalToll = tileConfig.toll * getPropertyLevel();
			// Utils::displayDialogue("player_action.moved.property_toll");
			std::cout << "You have to pay a toll of $" << totalToll << " to " << getOwner() << ".\n";

			if (player.getMoney() < totalToll)
			{
				std::cout << "You don't have enough money to pay the toll!\n";
				player.subtractMoney(player.getMoney());
			}
			else
			{
				player.subtractMoney(totalToll);

				// Find the owner player and add the toll to their money
				std::vector<Player> &players = game.getPlayers();
				for (Player &p : players)
				{
					if (p.getName() == getOwner())
					{
						p.addMoney(totalToll);
						break;
					}
				}

				std::cout << "You paid $" << totalToll << " to " << getOwner() << ".\n";
			}
			Utils::pressEnterToContinue();
		}
		else if (getOccupied() && getOwner() == player.getName())
		{
			bool chose = 0;
			while (!chose)
			{
				char choice;
				std::cout << tileConfig.id << " " << tileConfig.name << "\n";
				std::cout << "Upgrade Price:" << tileConfig.price << "\n";
				std::cout << "Sell Price:" << propertyLevel * tileConfig.price << "\n";
				Utils::displayDialogue("player_action.moved.property_owned");
				std::cout << "> ";
				std::cin >> choice;

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

				if (choice == 'R' || choice == 'r')
				{
					chose = 1;
					if (propertyLevel < 3)
					{
						GameConfig &config = GameConfig::getInstance();
						auto boardTiles = config.getBoardTiles();
						auto it = std::find_if(boardTiles.begin(), boardTiles.end(), [index](const TileConfig &tile)
											   { return tile.id == index; });

						if (it != boardTiles.end())
						{
							const TileConfig &tileConfig = *it;
							if (player.getMoney() >= tileConfig.price)
							{
								player.subtractMoney(tileConfig.price);
								setPropertyLevel(getPropertyLevel() + 1);
								std::cout << "Property upgraded to level " << getPropertyLevel() << "!\n";
							}
							else
							{
								std::cout << "You don't have enough money to upgrade this property.\n";
							}
						}
					}
					else
					{
						chose = 0;
						std::cout << "This property is already at the maximum level (3).\n";
					}
				}
				else if (choice == 'I' || choice == 'i')
				{
					player.showInfo();

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
						cardChoice = 0; // Set a default value to cancel
					}
					else
					{
						std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
					}

					if (cardChoice > 0 && cardChoice <= player.getCards().size())
					{
						Card chosenCard = player.getCards()[cardChoice - 1];
						std::vector<Player> &players = game.getPlayers();
						chosenCard.applyEffect(player, players, map);
					}
					else
					{
						std::cout << "Returning to game.\n";
					}
				}
				else if (choice == 'S' || choice == 's')
				{
					chose = 1;
					int sellPrice = propertyLevel * tileConfig.price;
					propertyLevel = 1;

					player.addMoney(sellPrice);
					player.removeProperty(player.getX(), player.getY());

					setOccupied(false);
					setOwner("");

					std::cout << "You sold this property for $" << sellPrice << ".\n";
				}
				else
				{
					std::cout << "You chose not to upgrade or sell the property.\n";
					chose = 1;
				}
			}

			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
		}
	}
	else
	{
		std::cout << player.getName() << " landed on a non-playable tile!\n";
	}
}

void Tile::enterShop(Game &game, Player &player)
{
	GameConfig &config = GameConfig::getInstance();
	std::vector<CardConfig> cards = config.getCards();

	std::cout << "=== Welcome to the Card Store ===\n";

	// Display all available cards
	for (size_t i = 0; i < cards.size(); i++)
	{
		std::cout << "[" << (i + 1) << "] " << cards[i].name
				  << " - Price: $" << cards[i].price
				  << " - Effect: " << cards[i].effect << "\n";
	}

	std::cout << "[" << (cards.size() + 1) << "] Exit store\n";
	std::cout << "Enter the number of the card you want to buy: ";

	bool chose = 1;
	do
	{
		int choice;
		chose = 1;
		std::cin >> choice;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}
		else
		{
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}

		if (choice > 0 && choice <= cards.size())
		{
			// User selected a card
			const CardConfig &selectedCard = cards[choice - 1];

			if (player.getMoney() >= selectedCard.price)
			{
				player.subtractMoney(selectedCard.price);
				Card card(selectedCard.name);
				player.addCard(card);
				std::cout << "You bought a " << selectedCard.name << ".\n";
			}
			else
			{
				std::cout << "You don't have enough money to buy this card.\n";
			}
		}
		else if (choice == cards.size() + 1)
		{
			std::cout << "Exiting the store.\n";
		}
		else
		{
			std::cout << "Invalid choice. Please try again.\n";
			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());

			// Redisplay the store menu
			std::cout << "=== Welcome to the Card Store ===\n";
			for (size_t i = 0; i < cards.size(); i++)
			{
				std::cout << "[" << (i + 1) << "] " << cards[i].name
						  << " - Price: $" << cards[i].price
						  << " - Effect: " << cards[i].effect << "\n";
			}
			std::cout << "[" << (cards.size() + 1) << "] Exit store\n";
			std::cout << "Enter the number of the card you want to buy: ";

			chose = 0;
		}
	} while (chose == 0);
}
