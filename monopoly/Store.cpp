#include "Store.h"
#include "Card.h"
#include <iostream>

void Store::open(Player& player) {  // Random store example
	std::cout << "--- Store ---\n";
	std::cout << "1. Dice Control Card ($1000)\n";
	std::cout << "2. Special Card ($1500)\n";
	std::cout << "0. Exit\n";

	int choice;
	std::cin >> choice;
	std::cin.ignore();

	if (choice == 1 && player.getMoney() >= 1000) {
		player.subtractMoney(1000);
		player.addCard(Card("Dice Control"));
		std::cout << "Bought Dice Control Card!\n";
	}
	else if (choice == 2 && player.getMoney() >= 1500) {
		player.subtractMoney(1500);
		player.addCard(Card("Special"));
		std::cout << "Bought Special Card!\n";
	}
	else if (choice == 0) {
		std::cout << "Leaving store.\n";
	}
	else {
		std::cout << "Not enough money or invalid choice.\n";
	}
}
