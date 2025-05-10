#ifndef TILE_H
#define TILE_H

#include <iostream>
#include "Game/Player.h"

class Tile {
private:
	char symbol;
	bool isOccupied;

public:
	Tile();
	Tile(char symbol);

	char getSymbol() const;
	void setSymbol(char newSymbol);
	bool getOccupied() const;
	void setOccupied(bool occupied);

	// Event handling methods for random events, etc.
	void handleEvent(Player& player);
	void enterShop(Player& player);
};

#endif
