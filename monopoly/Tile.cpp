#include "Tile.h"
#include "Player.h"
#include <iostream>

Tile::Tile() : symbol(' '), isOccupied(false) {}

Tile::Tile(char symbol) : symbol(symbol), isOccupied(false) {}

char Tile::getSymbol() const {
	return symbol;
}

void Tile::setSymbol(char newSymbol) {
	symbol = newSymbol;
}

bool Tile::getOccupied() const {
	return isOccupied;
}

void Tile::setOccupied(bool occupied) {
	isOccupied = occupied;
}

void Tile::handleEvent(Player& player) {
	// Check player position using X and Y coordinates
	int x = player.getX();
	int y = player.getY();

	// Check if the player has landed on a special tile based on coordinates
	if (x == 0 && y == 0) {
		std::cout << player.getName() << " landed on the Start tile!\n";
		// Handle start tile logic here
	}
	else if (x == 0 && y == 3) {
		std::cout << player.getName() << " landed on the Item Shop tile!\n";
		// Handle item shop logic here
	}
	else if (x == 7 && y == 3) {
		std::cout << player.getName() << " landed on the Item Shop tile!\n";
		// Handle item shop logic here
	}
	else if (x == 7 && y == 7) {
		std::cout << player.getName() << " landed on a Chance tile!\n";
		// Handle chance tile logic here
	}
	else if (x == 3 && y == 7) {
		std::cout << player.getName() << " landed on a Chance tile!\n";
		// Handle chance tile logic here
	}
	else if (x == 0 || x == 7 || y == 0 || y == 7) {
		// These are still "edge" tiles
		std::cout << player.getName() << " landed on a Property tile!\n";
		// Handle property logic here
	}
	else {
		std::cout << player.getName() << " landed on a non-playable tile!\n";
	}
}
