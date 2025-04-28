#include "Player.h"
#include <iostream>

Player::Player(const std::string& name) : name(name), money(5000), x(0), y(0), houseCount(0) {}

void Player::showInfo() const {  // Player information shown here
	std::cout << "Player: " << name << "\n";
	std::cout << "Money: $" << money << "\n";
	std::cout << "Houses: " << houseCount << "\n";
	std::cout << "Cards: ";

	for (const auto& card : cards) {
		std::cout << card.getType() << " ";
	}
	std::cout << "\n";
}

std::string Player::getName() const {
	return name;
}

void Player::move(int diceRoll) {
	// Get the player's current position
	int currentX = getX();
	int currentY = getY();

	std::vector<std::pair<int, int>> outerGrid = {
		{0,0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0},		// Top row
		{7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 7},				// Right column
		{6, 7}, {5, 7}, {4, 7}, {3, 7}, {2, 7}, {1, 7}, {0, 7},				// Bottom row
		{0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}                      // Left column
	};

	// Find the index of the current position in the outer grid
	int currentIndex = -1;
	for (int i = 0; i < outerGrid.size(); ++i) {
		if (outerGrid[i] == std::make_pair(currentX, currentY)) {
			currentIndex = i;
			break;
		}
	}

	int newIndex = (currentIndex + diceRoll) % outerGrid.size();

	int newX = outerGrid[newIndex].first;
	int newY = outerGrid[newIndex].second;
	setPosition(newX, newY);
}

void Player::addMoney(int amount) {
	money += amount;
}

void Player::subtractMoney(int amount) {
	money -= amount;
}

int Player::getMoney() const {
	return money;
}

void Player::addHouse() {
	houseCount++;
}

int Player::getHouseCount() const {
	return houseCount;
}

void Player::addCard(const Card& card) {
	cards.push_back(card);
}

int Player::getX() const {
	return x;
}

int Player::getY() const {
	return y;
}

void Player::setPosition(int newX, int newY) {
	x = newX;
	y = newY;
	std::cout << name << " moved to position (" << x << ", " << y << ").\n";
}
