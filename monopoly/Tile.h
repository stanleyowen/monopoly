#pragma once

#include <iostream>
#include "Game/Player.h"

class Tile
{
private:
	char symbol;
	bool isOccupied;
	std::string owner;

public:
	Tile();
	Tile(char symbol);

	char getSymbol() const;
	void setSymbol(char newSymbol);
	bool getOccupied() const;
	void setOccupied(bool occupied);

	std::string getOwner() const;
	void setOwner(const std::string &newOwner);

	// Event handling methods for random events, etc.
	void handleEvent(Player &player);
	void enterShop(Player &player);
};
