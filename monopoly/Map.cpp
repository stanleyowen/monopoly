#include "Map.h"
#include <iostream>
#include <iomanip>

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
}

void Map::drawBoard(const std::vector<Player>& players) const {
	std::cout << "+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+\n";
	for (int i = 0; i < 8; ++i) {
		// Print the row of tiles
		for (int j = 0; j < 8; ++j) {
			bool playerPrinted = false;
			for (const auto& player : players) {
				if (player.getX() == i && player.getY() == j) {
					std::cout << "| " << std::setw(6) << "[" + std::string(1, player.getSymbol()) + "]" << std::setw(10);
					playerPrinted = true;
					break;
				}
			}
			if (!playerPrinted) {
				int tileNumber = -1;
				if (i == 0) tileNumber = j;                        // Top row
				else if (j == 7) tileNumber = 7 + i;               // Right column
				else if (i == 7) tileNumber = 7 + 7 + (7 - j);     // Bottom row
				else if (j == 0) tileNumber = 7 + 7 + 7 + (7 - i); // Left column

				if (tileNumber != -1) {
					std::cout << "| " << std::setw(5) << tileNumber << " " << board[i][j].getSymbol() << std::setw(6) << " "; // Properly center tile number
				}
				else {
					std::cout << "| " << std::setw(13) << " "; // Empty tile space
				}
			}
		}
		std::cout << "|\n";

		// Empty lines for padding
		for (int k = 0; k < 2; ++k) {
			for (int j = 0; j < 8; ++j) {
				std::cout << "|              "; // Padding
			}
			std::cout << "|\n";
		}

		// Bottom border of each row
		std::cout << "+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+\n";
	}
}
