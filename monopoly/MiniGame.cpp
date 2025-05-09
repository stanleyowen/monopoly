#include "MiniGame.h"
#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

void MiniGame::playHorseRace(Player& player) {
	clearScreen();
	std::cout << "--- MiniGame: Horse Race ---\n";
	std::cout << "Four horses will race. Choose one to bet on (1-4): ";

	int betHorse = 0;
	while (betHorse < 1 || betHorse > 4) {
		std::cin >> betHorse;
		if (betHorse < 1 || betHorse > 4) std::cout << "Invalid horse. Pick 1 to 4: ";
	}

	const int finishLine = 50;
	int positions[4] = { 0, 0, 0, 0 };

	bool finished = false;
	int winner = -1;

	srand(static_cast<unsigned>(time(nullptr)));

	while (!finished) {
		clearScreen();
		std::cout << "--- Horse Race ---\n";

		for (int i = 0; i < 4; ++i) {
			int step = rand() % 5 + 1; // 1 to 5 steps
			positions[i] += step;
			if (positions[i] >= finishLine && winner == -1) {
				winner = i + 1;
				finished = true;
			}
			std::cout << "Horse " << (i + 1) << ": ";
			for (int j = 0; j < positions[i] && j < finishLine; ++j) std::cout << "=";
			std::cout << ">\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	std::cout << "\nHorse " << winner << " wins the race!\n";

	if (betHorse == winner) {
		std::cout << "You won the bet! You gain $1000!\n";
		player.addMoney(1000);
	}
	else {
		std::cout << "You lost the bet. You lose $500.\n";
		player.subtractMoney(500);
	}

	std::cout << "Press Enter to return to the game...";
	std::cin.ignore();
	std::cin.get();
	clearScreen();
}
