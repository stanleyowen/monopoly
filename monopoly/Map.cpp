#include "Map.h"
#include <iostream>

Map::Map() {}

void Map::setTile(int x, int y, char tileSymbol) {
	board[x][y].setSymbol(tileSymbol);
}

Tile& Map::getTile(int x, int y) {
	return board[x][y];
}

void Map::setupBoard() {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (i == 0 && j == 0) {
				setTile(i, j, 'S');
			}
			else if (i == 0 && j == 3) {
				setTile(i, j, 'I');
			}
			else if (j == 7 && i == 3) {
				setTile(i, j, 'C');
			}
			else if (i == 7 && j == 4) {
				setTile(i, j, 'C');
			}
			else {
				setTile(i, j, 'P');
			}
		}
	}

	// Inner empty spaces
	for (int i = 1; i < 7; ++i) {
		for (int j = 1; j < 7; ++j) {
			setTile(i, j, ' ');
		}
	}

	// Drawing the board right after setting it up
	drawBoard();
}

void Map::drawBoard() const {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			// Only border tiles have numbers
			int tileNumber = -1;
			if (i == 0) {
				tileNumber = j;
			}
			else if (j == 7) {
				tileNumber = 7 + i;
			}
			else if (i == 7) {
				tileNumber = 7 + 7 + (7 - j);
			}
			else if (j == 0) {
				tileNumber = 7 + 7 + 7 + (7 - i);
			}

			if (tileNumber != -1) {
				std::cout << "[" << tileNumber << "]" << board[i][j].getSymbol() << "\t";
			}
			else {
				std::cout << " " << " \t";
			}
		}
		std::cout << "\n";
	}
}

