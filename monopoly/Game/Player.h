#pragma once
#include <string>
#include <vector>
#include "../Card.h"

class Player
{
private:
	int x, y;
	std::string name;
	std::string symbol;
	std::string color;

	int money;
	int houseCount;

	std::vector<std::pair<int, int>> ownedTiles; // stores (x, y) coords of owned properties
	std::vector<Card> cards;

	int nextDiceValue = 0;

	bool inHospital = false;
	int hospitalTurnsLeft = 0;

public:
	Player(const std::string& name);
	Player(const std::string& name, std::string symbol, int startMoney);

	void showInfo() const;
	void move(int steps);
	void addMoney(int amount);
	void subtractMoney(int amount);
	int getMoney() const;
	void setColor(const std::string& color);
	std::string getColor() const;

	std::string getSymbol() const { return symbol; };
	void addHouse();
	int getHouseCount() const;

	void addProperty(int x, int y);
	const std::vector<std::pair<int, int>>& getProperties() const;

	const std::vector<Card>& getCards() const;
	void addCard(const Card& card);

	int getX() const;
	int getY() const;
	int getPositionId() const;
	void setPosition(int newX, int newY);

	std::string getName() const;

	void setNextDiceValue(int value);
	int getNextDiceValue() const;
	bool hasNextDiceValue() const;
	void clearNextDiceValue();
	void removeCard(const std::string& cardType);
	void removeProperty(int x, int y);

	void sendToHospital(int turns = 3);
	void setInHospital(bool inHospital);
	void leaveHospital();
	bool isInHospital() const;
	int getHospitalTurnsLeft() const;
	void setHospitalTurnsLeft(int turns);
	void decrementHospitalTurns();
};
