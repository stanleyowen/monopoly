#pragma once
#include <string>
#include <vector>
#include "Card.h"

class Player {
public:
	Player(const std::string& name);

	void showInfo() const;
	void move(int steps);
	void addMoney(int amount);
	void subtractMoney(int amount);
	int getMoney() const;

	void addHouse();
	int getHouseCount() const;

	void addCard(const Card& card);

	int getX() const;
	int getY() const;
	void setPosition(int newX, int newY);

	std::string getName() const;

private:
	int x, y;
	std::string name;
	int money;
	int houseCount;
	std::vector<Card> cards;
};
