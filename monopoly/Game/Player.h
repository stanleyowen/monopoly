#pragma once
#include <string>
#include <vector>
#include "Card.h"

class Player
{
public:
	Player(const std::string &name);
	Player(const std::string &name, std::string symbol, int startMoney);

	void showInfo() const;
	void move(int steps);
	void addMoney(int amount);
	void subtractMoney(int amount);
	int getMoney() const;

	std::string getSymbol() const { return symbol; };
	void addHouse();
	int getHouseCount() const;

	void addProperty(int x, int y);
	const std::vector<std::pair<int, int>> &getProperties() const;
	const std::vector<Card> &getCards() const;

	void addCard(const Card &card);

	int getX() const;
	int getY() const;
	void setPosition(int newX, int newY);

	std::string getName() const;

private:
	int x, y;
	std::string name;
	std::string symbol;
	int money;
	int houseCount;
	std::vector<std::pair<int, int>> ownedTiles; // stores (x, y) coords of owned properties
	std::vector<Card> cards;
};
