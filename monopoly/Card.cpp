#include "Card.h"
#include "Player.h"
#include <iostream>

Card::Card(const std::string& type) : type(type) {}

std::string Card::getType() const {
	return type;
}

void Card::applyEffect(Player& player, std::vector<std::shared_ptr<Player>>& players) {
	if (type == "Barrier Card") {
		std::cout << "Placing a barrier on a tile to block players.\n";
		// Implement barrier logic here
	}
	else if (type == "Dice Card") {
		std::cout << "Choose the number you roll on the dice.\n";
		// Implement dice control logic here

	}
	else if (type == "Destroy Card") {
		std::cout << "Destroying another player's property.\n";
		// Implement property destruction logic here

	}
	else if (type == "Fate Card") {
		std::cout << "Triggering a Fate event.\n";
		// Implement fate event logic here
		int fate = rand() % 2;
		if (fate == 0) {
			std::cout << "You gained $500.\n";
			player.addMoney(500);
		}
		else {
			std::cout << "You lost $300.\n";
			player.subtractMoney(300);
		}
	}
	else if (type == "Rocket Card") {
		std::cout << "Sending a player to the hospital for 2 turns.\n";
		// Implement rocket card logic here

	}
	else {
		std::cout << "Unknown card type.\n";
	}
}
