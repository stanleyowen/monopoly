#pragma once

#include <iostream>
#include "Game/Player.h"

class Tile
{
private:
	char symbol;
	bool isOccupied;
	int propertyLevel = 1; // 1-3
	std::string owner;
	int positionId;  // ·s¼W¦ì¸m ID

public:
	Tile();
	Tile(char symbol);

	char getSymbol() const;
	void setSymbol(char newSymbol);
	bool getOccupied() const;
	void setOccupied(bool occupied);
	int getPropertyLevel() const;
	void setPropertyLevel(int level);
	int getPositionId() const;

	std::string getOwner() const;
	void setOwner(const std::string& newOwner);

	// Event handling methods for random events, etc.
	void handleEvent(Player& player, Map& map);
	void enterShop(Player& player);


};
