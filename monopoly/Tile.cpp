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

void Tile::setOwner(const std::string& newOwner)
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

void Tile::handleEvent(Player& player, Map& map)
{
	GameConfig& config = GameConfig::getInstance();
	Game& game = Game::getInstance();

	auto boardTiles = config.getBoardTiles();

	// Get the player's current position as an index
	int index = player.getPositionId();

	// Find the tile configuration based on the player's index
	auto it = std::find_if(boardTiles.begin(), boardTiles.end(), [index](const TileConfig& tile)
		{
			return tile.id == index; // Match the index directly
		});

	if (it == boardTiles.end())
	{
		std::cout << "Invalid tile configuration for index " << index << ".\n";
		return;
	}

	const TileConfig& tileConfig = *it;

	std::cout << player.getName() << " landed on " << tileConfig.name << "!\n";

	if (tileConfig.type == "start")
	{
		player.addMoney(config.getPassingStartBonus());
		std::cout << "You received a bonus of $" << config.getPassingStartBonus() << "!\n";
	}
	else if (tileConfig.type == "store")
	{
		bool chose = 0;
		while (!chose) {
			Utils::displayDialogue("player_action.moved.store");
			char choice;
			std::cout << "> ";
			std::cin >> choice;
			std::cin.ignore();
			if (choice == 'E' || choice == 'e')
			{
				enterShop(player);
				chose = 1;
			}
			else if (choice == 'I' || choice == 'i') {
				player.showInfo();

				int cardChoice;
				std::cout << "Enter the card number to use (0 to cancel): ";
				std::cin >> cardChoice;
				std::cin.ignore();

				if (cardChoice > 0 && cardChoice <= player.getCards().size())
				{
					Card chosenCard = player.getCards()[cardChoice - 1];
					std::vector<Player>& players = game.getPlayers();
					chosenCard.applyEffect(player, players, map);
					game.getMap().drawBoard(game.getPlayers());

				}
				else {
					std::cout << "Returning to game.\n";
					Utils::clearScreen();
					game.getMap().drawBoard(game.getPlayers());
				}
			}
			else
			{
				chose = 1;
				std::cout << "You chose to pass.\n";
			}
		}
	}
	else if (tileConfig.type == "fate")
	{
		std::cout << "You triggered a Fate tile...\n\n";

		int roll = rand() % 100;

		if (roll < 25)
		{
			std::cout << "[Fate] A minigame has been triggered!\n\n";
			int gameType = rand() % 4;
			if (gameType == 0) MiniGame::playHorseRace(player);
			else if (gameType == 1) MiniGame::playDragonGate(player);
			else if (gameType == 2) MiniGame::playMazeEscape(player);
			else MiniGame::playTreasureHunt(player);

			game.getMap().drawBoard(game.getPlayers());
		}
		else if (roll < 95) // 25–94 → 70% chance
		{
			GameConfig& config = GameConfig::getInstance();
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
				"You found $",
				"You sold old stuff and earned $",
				"You received a mystery gift worth $"
			};
			std::vector<std::string> lossMsgs = {
				"You paid a fine of $",
				"You lost your wallet and lost $",
				"You bought a useless app for $"
			};

			int msgIndex = rand() % 3;
			std::string message = gain
				? gainMsgs[msgIndex] + std::to_string(actualAmount) + "."
				: lossMsgs[msgIndex] + std::to_string(-actualAmount) + ".";

			std::cout << "[Fate] " << message << "\n\n";

			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
		}
		else
		{
			std::cout << "[Fate] But nothing happened...\n\n";

			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
		}
	}
	else if (tileConfig.type == "chance")
	{
		std::cout << "You triggered a Chance tile...\n\n";

		int roll = rand() % 100;

		if (roll < 35)
		{
			std::cout << "[Chance] A minigame has been triggered!\n\n";
			int gameType = rand() % 4;
			if (gameType == 0) MiniGame::playHorseRace(player);
			else if (gameType == 1) MiniGame::playDragonGate(player);
			else if (gameType == 2) MiniGame::playMazeEscape(player);
			else MiniGame::playTreasureHunt(player);

			game.getMap().drawBoard(game.getPlayers());
		}
		else if (roll < 90) // 35–89 → 55% chance
		{
			GameConfig& config = GameConfig::getInstance();
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
				"Your stocks exploded! You gained $"
			};
			std::vector<std::string> lossMsgs = {
				"You got scammed and lost $",
				"You paid unexpected hospital bills of $",
				"Your car broke down. Repairs cost $",
				"You invested in crypto at the wrong time. Lost $"
			};

			int msgIndex = rand() % 4;
			std::string message = gain
				? gainMsgs[msgIndex] + std::to_string(actualAmount) + "!"
				: lossMsgs[msgIndex] + std::to_string(-actualAmount) + "...";

			std::cout << "[Chance] " << message << "\n\n";

			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
		}
		else
		{
			std::cout << "[Chance] But nothing happened...\n\n";

			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
		}
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
			while (!chose) {
				char choice;
				std::cout << tileConfig.id << " " << tileConfig.name << " Price:" << tileConfig.price << "\n";
				Utils::displayDialogue("player_action.moved.property_unowned");
				std::cout << "> ";
				std::cin >> choice;
				std::cin.ignore();
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
				else if (choice == 'I' || choice == 'i') {
					player.showInfo();

					int cardChoice;
					std::cout << "Enter the card number to use (0 to cancel): ";
					std::cin >> cardChoice;
					std::cin.ignore();

					if (cardChoice > 0 && cardChoice <= player.getCards().size())
					{
						Card chosenCard = player.getCards()[cardChoice - 1];
						std::vector<Player>& players = game.getPlayers();
						chosenCard.applyEffect(player, players, map);
						game.getMap().drawBoard(game.getPlayers());

					}
					else {
						std::cout << "Returning to game.\n";
						Utils::clearScreen();
						game.getMap().drawBoard(game.getPlayers());
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
			//Utils::displayDialogue("player_action.moved.property_toll");
			std::cout << "You have to pay a toll of $" << totalToll << " to " << getOwner() << ".\n";

			if (player.getMoney() < totalToll)
			{
				std::cout << "You don't have enough money to pay the toll!\n";
				player.subtractMoney(player.getMoney());
			}
			else
			{
				player.subtractMoney(totalToll);
				std::cout << "You paid $" << totalToll << " to " << getOwner() << ".\n";
			}
			Utils::pressEnterToContinue();
		}
		else if (getOccupied() && getOwner() == player.getName())
		{
			bool chose = 0;
			while (!chose) {
				char choice;
				std::cout << tileConfig.id << " " << tileConfig.name << "\n";
				std::cout << "Upgrade Price:" << tileConfig.price << "\n";
				std::cout << "Sell Price:" << propertyLevel * tileConfig.price << "\n";
				Utils::displayDialogue("player_action.moved.property_owned");
				std::cout << "> ";
				std::cin >> choice;
				std::cin.ignore();

				if (choice == 'R' || choice == 'r')
				{
					chose = 1;
					if (propertyLevel < 3)
					{
						GameConfig& config = GameConfig::getInstance();
						auto boardTiles = config.getBoardTiles();
						auto it = std::find_if(boardTiles.begin(), boardTiles.end(), [index](const TileConfig& tile)
							{ return tile.id == index; });

						if (it != boardTiles.end())
						{
							const TileConfig& tileConfig = *it;
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
				else if (choice == 'I' || choice == 'i') {
					player.showInfo();

					int cardChoice;
					std::cout << "Enter the card number to use (0 to cancel): ";
					std::cin >> cardChoice;
					std::cin.ignore();

					if (cardChoice > 0 && cardChoice <= player.getCards().size())
					{
						Card chosenCard = player.getCards()[cardChoice - 1];
						std::vector<Player>& players = game.getPlayers();
						chosenCard.applyEffect(player, players, map);
						game.getMap().drawBoard(game.getPlayers());

					}
					else {
						std::cout << "Returning to game.\n";
						Utils::clearScreen();
						game.getMap().drawBoard(game.getPlayers());
					}
				}
				else if (choice == 'S' || choice == 's')
				{
					chose = 1;
					int sellPrice = propertyLevel * tileConfig.price;
					propertyLevel = 1;

					player.addMoney(sellPrice);
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

		}
	}
	else
	{
		std::cout << player.getName() << " landed on a non-playable tile!\n";
	}
}

void Tile::enterShop(Player& player)
{
	std::cout << "=== Welcome to the Card Store ===\n";
	std::cout << "[1] Barrier Card    - Price: $1500       - Effect: Place a barrier on a tile to block players.\n";
	std::cout << "[2] Dice Card       - Price: $2000       - Effect: Choose the number you roll on the dice.\n";
	std::cout << "[3] Destroy Card    - Price: $2500       - Effect: Destroy another player's property.\n";
	std::cout << "[4] Fate Card       - Price: $1000       - Effect: Trigger a Fate event.\n";
	std::cout << "[5] Rocket Card     - Price: $3000       - Effect: Send a player to the hospital for 2 turns.\n";
	std::cout << "[0] Exit store\n";
	std::cout << "Enter the number of the card you want to buy: ";

	bool chose = 1;
	do {
		int choice;
		chose = 1;
		std::cin >> choice;
		std::cin.ignore();
		switch (choice)
		{
		case 1:
			if (player.getMoney() >= 1500)
			{
				player.subtractMoney(1500);
				Card barrierCard("Barrier Card");
				player.addCard(barrierCard);
				std::cout << "You bought a Barrier Card.\n";
			}
			else
			{
				std::cout << "You don't have enough money to buy this card.\n";
			}
			break;
		case 2:
			if (player.getMoney() >= 2000)
			{
				player.subtractMoney(2000);
				Card diceCard("Dice Card");
				player.addCard(diceCard);
				std::cout << "You bought a Dice Card.\n";
			}
			else
			{
				std::cout << "You don't have enough money to buy this card.\n";
			}
			break;
		case 3:
			if (player.getMoney() >= 2500)
			{
				player.subtractMoney(2500);
				Card destroyCard("Destroy Card");
				player.addCard(destroyCard);
				std::cout << "You bought a Destroy Card.\n";
			}
			else
			{
				std::cout << "You don't have enough money to buy this card.\n";
			}
			break;
		case 4:
			if (player.getMoney() >= 1000)
			{
				player.subtractMoney(1000);
				Card fateCard("Fate Card");
				player.addCard(fateCard);
				std::cout << "You bought a Fate Card.\n";
			}
			else
			{
				std::cout << "You don't have enough money to buy this card.\n";
			}
			break;
		case 5:
			if (player.getMoney() >= 3000)
			{
				player.subtractMoney(3000);
				Card rocketCard("Rocket Card");
				player.addCard(rocketCard);
				std::cout << "You bought a Rocket Card.\n";
			}
			else
			{
				std::cout << "You don't have enough money to buy this card.\n";
			}
			break;
		case 0:
			std::cout << "Exiting the store.\n";
			break;
		default:
			std::cout << "Invalid choice. Exiting the store.\n";
			std::cout << "Enter the number of the card you want to buy: ";
			chose = 0;
			break;
		}
	} while (chose == 0);
	Utils::pressEnterToContinue();
}
