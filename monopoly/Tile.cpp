#include "Tile.h"
#include "Game/Player.h"
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

void Tile::handleEvent(Player &player)
{
	std::cout << "Debug: Player " << player.getName() << " interacting with Tile. Symbol: " << symbol
			  << ", isOccupied: " << isOccupied << "\n";

	int x = player.getX();
	int y = player.getY();

	if (x == 0 && y == 0)
	{
		std::cout << player.getName() << " landed on the Start tile!\n";
		player.addMoney(200);
	}
	else if (x == 0 && y == 3)
	{
		std::cout << player.getName() << " landed on the Item Shop tile!\n";
		std::cout << "Please choose an action:\n";
		std::cout << "E: Enter the shop\n";
		std::cout << "I: View Player Info\n";
		std::cout << "*: Press any key to PASS\n";

		char choice;
		std::cout << "> ";
		std::cin >> choice;

		if (choice == 'E' || choice == 'e')
		{
			enterShop(player);
		}
		else if (choice == 'I' || choice == 'i')
		{
			player.showInfo();
		}
		else
		{
			std::cout << "You chose to pass.\n";
		}
	}
	else if ((x == 7 && y == 7) || (x == 3 && y == 7) || (x == 0 && y == 2) || (x == 7 && y == 5))
	{
		std::cout << player.getName() << " landed on a ";
		if ((x == 0 && y == 2) || (x == 7 && y == 5))
			std::cout << "Fate";
		else
			std::cout << "Chance";
		std::cout << " tile!\n";

		int rng = rand() % 3;
		if (rng == 0)
		{
			MiniGame::playHorseRace(player);
		}
		else if (rng == 1)
		{
			MiniGame::playDragonGate(player);
		}
		else
		{
			std::cout << "Nothing happens... this time.\n";
		}
	}
	else if (x == 0 || x == 7 || y == 0 || y == 7)
	{
		std::cout << player.getName() << " landed on a Property tile!\n";
		if (!getOccupied())
		{
			char choice;
			std::cout << "This property is available for purchase.\n";
			std::cout << "Do you want to buy this property for $100? (Y/N): ";

			while (std::cin >> choice)
			{
				if (choice == 'Y' || choice == 'y')
				{
					if (player.getMoney() >= 100)
					{
						player.subtractMoney(100);
						player.addProperty(x, y);
						setOccupied(true);
						std::cout << "You have purchased this property.\n";
					}
					else
					{
						std::cout << "You don't have enough money to buy this property.\n";
					}
					break;
				}
				else if (choice == 'N' || choice == 'n')
				{
					std::cout << "You chose not to buy the property.\n";
					break;
				}
				else if (choice != '\n') // Ignore newline characters
				{
					std::cout << "Invalid input. Please enter 'Y' or 'N': ";
				}
			}
		}
		else if (std::find(player.getProperties().begin(), player.getProperties().end(), std::make_pair(x, y)) == player.getProperties().end())
		{
			std::cout << "This property is already owned by another player. Pay rent.\n";
			player.subtractMoney(50);
		}
		else
		{
			std::cout << "You already own this property!\n";
		}
	}
	else
	{
		std::cout << player.getName() << " landed on a non-playable tile!\n";
	}
}

void Tile::enterShop(Player &player)
{
	std::cout << "=== Welcome to the Card Store ===\n";
	std::cout << "[1] Barrier Card    - Price: $1500       - Effect: Place a barrier on a tile to block players.\n";
	std::cout << "[2] Dice Card       - Price: $2000       - Effect: Choose the number you roll on the dice.\n";
	std::cout << "[3] Destroy Card    - Price: $2500       - Effect: Destroy another player's property.\n";
	std::cout << "[4] Fate Card       - Price: $1000       - Effect: Trigger a Fate event.\n";
	std::cout << "[5] Rocket Card     - Price: $3000       - Effect: Send a player to the hospital for 2 turns.\n";
	std::cout << "[0] Exit store\n";
	std::cout << "Enter the number of the card you want to buy: ";

	int choice;
	std::cin >> choice;

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
		break;
	}
}
