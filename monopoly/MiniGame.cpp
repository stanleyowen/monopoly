#include "MiniGame.h"
#include <iostream>
#include <cstdlib>

void MiniGame::play(Player& player) {  // Random minigame example
	std::cout << "--- MiniGame: Number Guess ---\n";
	int number = rand() % 10 + 1;
	int guess;
	std::cout << "Guess a number between 1 and 10: ";
	std::cin >> guess;

	if (guess == number) {
		player.addMoney(1000);
		std::cout << "Correct! You win $1000!\n";
	}
	else {
		player.subtractMoney(500);
		std::cout << "Wrong! You lose $500!\n";
	}
}
