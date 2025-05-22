#include "Player.h"
#include "GameConfig.h"
#include <iostream>
#include <iomanip>

Player::Player(const std::string &name) : name(name), money(5000), x(0), y(0), houseCount(0) {}

Player::Player(const std::string &name, std::string symbol, int startMoney)
	: name(name), symbol(symbol), money(startMoney), x(0), y(0), houseCount(0) {}

void Player::showInfo() const
{
	std::cout << "\n"
			  << name << "'s Cards:\n";
	std::cout << "+----------------------------------------------------------------------------+\n";
	std::cout << "| No. | Card Name             | Effect                                       |\n";
	std::cout << "+----------------------------------------------------------------------------+\n";

	GameConfig &config = GameConfig::getInstance();
	std::vector<CardConfig> cardConfigs = config.getCards();

	for (size_t i = 0; i < cards.size(); ++i)
	{
		std::string cardName = cards[i].getType();
		std::string effect = "Unknown card effect.";

		// Look up the effect from the config
		for (const auto &cardConfig : cardConfigs)
		{
			if (cardConfig.name == cardName)
			{
				effect = cardConfig.effect;
				break;
			}
		}

		// Format line (fixed-width using std::setw)
		std::cout << "| " << std::setw(3) << (i + 1) << " | "
				  << std::setw(21) << std::left << cardName << " | "
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

int Player::getPositionId() const
{
	if (x == 0) // Top row
		return y;
	else if (y == 7) // Right column
		return 7 + x;
	else if (x == 7) // Bottom row
		return 7 + 7 + (7 - y);
	else if (y == 0) // Left column
		return 7 + 7 + 7 + (7 - x);

	return -1; // Invalid position (inside the board)
}

void Player::setPosition(int newX, int newY)
{
	x = newX;
	y = newY;
}

void Player::setColor(const std::string &color)
{
	this->color = color;
}

std::string Player::getColor() const
{
	return color;
}

void Player::removeCard(const std::string &cardType)
{
	auto it = std::find_if(cards.begin(), cards.end(), [&](const Card &card)
						   { return card.getType() == cardType; });

	if (it != cards.end())
	{
		cards.erase(it);
		std::cout << "Card removed: " << cardType << "\n";
	}
	else
	{
		std::cout << "Card not found: " << cardType << "\n";
	}
}

void Player::setNextDiceValue(int value)
{
	nextDiceValue = value;
}

int Player::getNextDiceValue() const
{
	return nextDiceValue;
}

bool Player::hasNextDiceValue() const
{
	return nextDiceValue != 0;
}

void Player::clearNextDiceValue()
{
	nextDiceValue = 0;
}

void Player::removeProperty(int x, int y)
{
	auto it = std::remove_if(ownedTiles.begin(), ownedTiles.end(),
							 [x, y](const std::pair<int, int> &coord)
							 {
								 return coord.first == x && coord.second == y;
							 });

	if (it != ownedTiles.end())
	{
		ownedTiles.erase(it, ownedTiles.end());
		std::cout << "Property at (" << x << ", " << y << ") removed from ownership.\n";
	}
}

void Player::sendToHospital(int turns)
{
	inHospital = true;
	hospitalTurnsLeft = turns;
}

void Player::setInHospital(bool val) { inHospital = val; }

void Player::setHospitalTurnsLeft(int turns) { hospitalTurnsLeft = turns; }

void Player::leaveHospital()
{
	inHospital = false;
	hospitalTurnsLeft = 0;
}

bool Player::isInHospital() const { return inHospital; }

int Player::getHospitalTurnsLeft() const { return hospitalTurnsLeft; }

void Player::decrementHospitalTurns()
{
	if (hospitalTurnsLeft > 0)
		--hospitalTurnsLeft;
	if (hospitalTurnsLeft == 0)
		inHospital = false;
}