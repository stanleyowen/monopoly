#include "Player.h"
#include <iostream>
#include <iomanip>

Player::Player(const std::string &name) : name(name), money(5000), x(0), y(0), houseCount(0) {}

Player::Player(const std::string &name, std::string symbol, int startMoney)
	: name(name), symbol(symbol), money(startMoney), x(0), y(0), houseCount(0) {}

void Player::showInfo() const
{
	std::cout << name << "'s Cards:\n";
	std::cout << "+----------------------------------------------------------------------------+\n";
	std::cout << "| No. | Card Name             | Effect                                       |\n";
	std::cout << "+----------------------------------------------------------------------------+\n";

	for (size_t i = 0; i < cards.size(); ++i)
	{
		std::string name = cards[i].getType(); // assuming getType() returns name
		std::string effect;

		// Match known effects � feel free to expand this list
		if (name == "Barrier Card")
			effect = "Place a barrier on a tile to blcok players..";
		else if (name == "Dice Card")
			effect = "Choose the number you roll on the dice.";
		else if (name == "Destroy Card")
			effect = "Destroy another player's property.";
		else if (name == "Fate Card")
			effect = "Trigger a Fate event.";
		else if (name == "Rocket Card")
			effect = "Send a player to the hospital for 2 turns.";
		else
			effect = "Unknown card effect.";

		// Format line (fixed-width using std::setw)
		std::cout << "| " << std::setw(3) << (i + 1) << " | "
				  << std::setw(21) << std::left << name << " | "
				  << std::setw(43) << std::left << effect << "|\n";
	}

	std::cout << "+----------------------------------------------------------------------------+\n";
}

std::string Player::getName() const
{
	return name;
}

void Player::move(int diceRoll)
{
	// Get the player's current position
	int currentX = getX();
	int currentY = getY();

	std::vector<std::pair<int, int>> outerGrid = {
		{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, // Top row (left to right)
		{1, 7},
		{2, 7},
		{3, 7},
		{4, 7},
		{5, 7},
		{6, 7},
		{7, 7}, // Right column (top to bottom)
		{7, 6},
		{7, 5},
		{7, 4},
		{7, 3},
		{7, 2},
		{7, 1},
		{7, 0}, // Bottom row (right to left)
		{6, 0},
		{5, 0},
		{4, 0},
		{3, 0},
		{2, 0},
		{1, 0} // Left column (bottom to top)
	};

	// Find the index of the current position in the outer grid
	int currentIndex = -1;
	for (int i = 0; i < outerGrid.size(); ++i)
	{
		if (outerGrid[i] == std::make_pair(currentX, currentY))
		{
			currentIndex = i;
			break;
		}
	}

	int newIndex = (currentIndex + diceRoll) % outerGrid.size();
	int newX = outerGrid[newIndex].first;
	int newY = outerGrid[newIndex].second;
	setPosition(newX, newY);
}

void Player::addMoney(int amount)
{
	money += amount;
}

void Player::subtractMoney(int amount)
{
	money -= amount;
}

int Player::getMoney() const
{
	return money;
}

void Player::addHouse()
{
	houseCount++;
}

int Player::getHouseCount() const
{
	return houseCount;
}

void Player::addProperty(int x, int y)
{
	ownedTiles.emplace_back(x, y);
}

const std::vector<std::pair<int, int>> &Player::getProperties() const
{
	return ownedTiles;
}

const std::vector<Card> &Player::getCards() const
{
	return cards;
}

void Player::addCard(const Card &card)
{
	cards.push_back(card);
}

int Player::getX() const
{
	return x;
}

int Player::getY() const
{
	return y;
}

void Player::setPosition(int newX, int newY)
{
	x = newX;
	y = newY;
	std::cout << name << " moved to position (" << x << ", " << y << ").\n";
}
