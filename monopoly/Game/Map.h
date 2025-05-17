#ifndef MAP_H
#define MAP_H

#include "Tile.h"
#include <iostream>

class Map {
private:
	Tile board[8][8];

public:
	Map();
	void setTile(int x, int y, char tileSymbol);
	Tile& getTile(int x, int y);

	void setupBoard();
	void drawBoard(const std::vector<Player>& players) const;

	void resetProperty(int x, int y);
	Tile &getTileById(int tileId);

	std::vector<Tile> &getTiles();
	const std::vector<Tile> &getTiles() const;
};

#endif // MAP_H
