#include "Map.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Map::Map() {}

void Map::setTile(int x, int y, char tileSymbol) {
	board[x][y].setSymbol(tileSymbol);
}

Tile& Map::getTile(int x, int y) {
	return board[x][y];
}

void Map::setupBoard() {
	// Set default to Property tile
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			setTile(i, j, 'P');
		}
	}

	// Empty inner tiles
	for (int i = 1; i < 7; ++i) {
		for (int j = 1; j < 7; ++j) {
			setTile(i, j, ' ');
		}
	}

	setTile(0, 0, 'S');  // Start
	setTile(0, 2, 'F');  // Fate
	setTile(0, 4, 'I');  // Item Shop
	setTile(3, 7, 'C');  // Chance
	setTile(7, 6, 'H');  // Hospital
	setTile(4, 0, 'C');  // Chance
	setTile(7, 5, 'F');  // Fate
}

void Map::drawBoard(const std::vector<Player>& players) const {
	// ANSI color codes
	const std::string RESET = "\033[0m";
	const std::string RED_BG = "\033[41m";
	const std::string GREEN_BG = "\033[42m";
	const std::string YELLOW_BG = "\033[43m";
	const std::string MAGENTA_BG = "\033[45m";
	const std::string CYAN_BG = "\033[46m";
	const std::string BLACK_BG = "\033[40m";
	const std::string BOLD = "\033[1m";

	auto getTileColor = [&](char sym) -> std::string {
		switch (sym) {
		case 'S': return GREEN_BG;
		case 'I': return YELLOW_BG;
		case 'C': return MAGENTA_BG;
		case 'H': return RED_BG;
		case 'F': return CYAN_BG;
		case 'P': return BLACK_BG;
		default: return BLACK_BG;
		}
	};

	auto getTileLabel = [&](char sym) -> std::string {
		switch (sym) {
		case 'S': return "START";
		case 'I': return "SHOP";
		case 'C': return "CHANCE";
		case 'H': return "HOSP";
		case 'F': return "FATE";
		case 'P': return "PROP";
		default:  return "????";
		}
	};

	std::cout << BOLD << "+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+" << RESET << "\n";
	for (int i = 0; i < 8; ++i) {
		// First row: tile number + label
		for (int j = 0; j < 8; ++j) {
			int tileNumber = -1;
			if (i == 0) tileNumber = j;
			else if (j == 7) tileNumber = 7 + i;
			else if (i == 7) tileNumber = 7 + 7 + (7 - j);
			else if (j == 0) tileNumber = 7 + 7 + 7 + (7 - i);

			char symbol = board[i][j].getSymbol();
			std::string tileColor = getTileColor(symbol);
			std::string label = getTileLabel(symbol);

			std::stringstream top;
			if (tileNumber != -1) {
				top << std::setw(2) << tileNumber << " " << label;
			}
			else {
				top << " ";
			}
			std::cout << "| " << tileColor << std::setw(13) << std::left << top.str() << RESET;
		}
		std::cout << "|\n";

		// Second row: player icons
		for (int j = 0; j < 8; ++j) {
			std::stringstream rawIcons;
			for (const auto& player : players) {
				if (player.getX() == i && player.getY() == j) {
					std::string color = (player.getSymbol() == 'A') ? "\033[31m" : "\033[34m";
					rawIcons << color << "[" << player.getSymbol() << "]" << RESET;
				}
			}

			std::string iconStr = rawIcons.str();
			int visualLength = 0;
			for (size_t k = 0; k < iconStr.size(); ++k) {
				if (iconStr[k] == '\033') {
					while (k < iconStr.size() && iconStr[k] != 'm') ++k;
				}
				else {
					++visualLength;
				}
			}

			int padLeft = (13 - visualLength) / 2;
			int padRight = 13 - visualLength - padLeft;
			std::cout << "| " << std::string(padLeft, ' ') << iconStr << std::string(padRight, ' ');
		}
		std::cout << "|\n";

		// Third row: empty visual padding
		for (int j = 0; j < 8; ++j) std::cout << "|              ";
		std::cout << "|\n";

		std::cout << BOLD << "+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+" << RESET << "\n";
	}
}


