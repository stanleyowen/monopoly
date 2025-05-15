#include "Card.h"
#include "Game/Player.h"
#include <iostream>

Card::Card(const std::string& type) : type(type) {}

std::string Card::getType() const {
	return type;
}

void Card::applyEffect(Player& player, std::vector<Player>& players)
{
	if (type == "Dice Card")
	{
		int diceValue;
		std::cout << "Choose your dice value (1-12): ";
		std::cin >> diceValue;

		if (diceValue >= 1 && diceValue <= 12)
		{
			player.setNextDiceValue(diceValue);
			std::cout << "Dice Control Card used! Next dice will roll: " << diceValue << "\n";
			player.removeCard("Dice Card");
		}
		else
		{
			std::cout << "Invalid dice value. Please enter a number between 1 and 12.\n";
		}
	}
	else
	{
		std::cout << "Unknown or unsupported card type.\n";
	}
}



/* void Card::applyEffect(Player& player, std::vector<std::shared_ptr<Player>>& players) {
	if (type == "Barrier Card") { //路障卡
		std::cout << "Placing a barrier on a tile to block players.\n";
		// Implement barrier logic here
	}
	else if (type == "Dice Card") { //骰控卡
		std::cout << "Choose the number you roll on the dice.\n";
		// Implement dice control logic here
		int diceValue;
		std::cout << "Choose your dice value (1-12): ";
		std::cin >> diceValue;

		if (diceValue >= 1 && diceValue <= 12) {
			player.setNextDiceValue(diceValue);
			std::cout << "Dice Control Card used! Next dice will roll: " << diceValue << "\n";
			player.removeCard("Dice Card");
		}
		else {
			std::cout << "Invalid dice value. Please enter a number between 1 and 12.\n";
		}
	}
	else if (type == "Destroy Card") { //拆除卡
		std::cout << "Destroying another player's property.\n";
		// Implement property destruction logic here

	}
	else if (type == "Fate Card") { //命運卡
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
	else if (type == "Rocket Card") { //火箭卡
		std::cout << "Sending a player to the hospital for 2 turns.\n";
		// Implement rocket card logic here

	}
	else {
		std::cout << "Unknown card type.\n";
	}
}*/
