#include "Utils.h"
#include "MiniGame.h"
#include "Game/Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>
#include <limits>

void MiniGame::playHorseRace(Player &player)
{
	Utils::clearScreen();
	std::cout << "--- MiniGame: Horse Race ---\n";
	std::cout << "Four horses will race. Choose one to bet on (1-4): ";

	int betHorse = 0;
	while (betHorse < 1 || betHorse > 4)
	{
		std::cin >> betHorse;
		if (betHorse < 1 || betHorse > 4)
			std::cout << "Invalid horse. Pick 1 to 4: ";
	}

	const int finishLine = 50;
	int positions[4] = {0, 0, 0, 0};

	bool finished = false;
	int winner = -1;

	srand(static_cast<unsigned>(time(nullptr)));

	while (!finished)
	{
		Utils::clearScreen();
		std::cout << "--- Horse Race ---\n";

		for (int i = 0; i < 4; ++i)
		{
			int step = rand() % 5 + 1; // 1 to 5 steps
			positions[i] += step;
			if (positions[i] >= finishLine && winner == -1)
			{
				winner = i + 1;
				finished = true;
			}
			std::cout << "Horse " << (i + 1) << ": ";
			for (int j = 0; j < positions[i] && j < finishLine; ++j)
				std::cout << "=";
			std::cout << ">\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	std::cout << "\nHorse " << winner << " wins the race!\n";

	if (betHorse == winner)
	{
		std::cout << "You won the bet! You gain $1000!\n";
		player.addMoney(1000);
	}
	else
	{
		std::cout << "You lost the bet. You lose $500.\n";
		player.subtractMoney(500);
	}

	Utils::pressEnterToContinue();
	Utils::clearScreen();
}

void MiniGame::playDragonGate(Player &player)
{
	Utils::clearScreen();
	srand(static_cast<unsigned>(time(nullptr)));
	std::cout << "--- MiniGame: Dragon Gate ---\n";
	int card1 = rand() % 13 + 1;
	int card2 = rand() % 13 + 1;
	std::cout << "First card: " << card1 << ", Second card: " << card2 << "\n";

	int wager = 0;
	while (true)
	{
		std::cout << "Enter your bet (up to $1000): ";
		std::cin >> wager;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // flush bad input
			std::cout << "Invalid input. Please enter a number.\n";
			continue;
		}

		if (wager <= 0)
		{
			std::cout << "Bet must be greater than zero.\n";
		}
		else if (wager > 1000)
		{
			std::cout << "Bet must not be greater than 1000.\n";
		}
		else
		{
			std::cout << "You bet $" << wager << ".\n";
			break;
		}
	}

	int nextCard = rand() % 13 + 1;
	bool correct = false;
	bool pillarHit = (nextCard == card1 || nextCard == card2);

	if (card1 != card2)
	{
		int low, high;
		if (card1 < card2)
		{
			low = card1;
			high = card2;
		}
		else
		{
			low = card2;
			high = card1;
		}

		std::string guess;
		std::cout << "Do you think the next card will be [inside] or [outside] the range? ";
		std::cin >> guess;

		std::cout << "Next card: " << nextCard << "\n";

		if (pillarHit)
		{
			std::cout << "Pillar hit! You lose 2x your bet!\n";
			player.subtractMoney(wager * 2);
			return;
		}

		if (guess == "inside")
		{
			correct = (nextCard > low && nextCard < high);
		}
		else
		{
			correct = (nextCard < low || nextCard > high);
		}

		if (correct)
		{
			std::cout << "You guessed right! You win your bet!\n";
			player.addMoney(wager);
		}
		else
		{
			std::cout << "Wrong guess! You lose your bet.\n";
			player.subtractMoney(wager);
		}
	}
	else
	{
		std::string guess;
		std::cout << "Cards are equal. Will the next card be [higher] or [lower]? ";
		std::cin >> guess;

		std::cout << "Next card: " << nextCard << "\n";

		if (pillarHit)
		{
			std::cout << "Pillar hit! You lose 3x your bet!\n";
			player.subtractMoney(wager * 3);
			return;
		}

		if (guess == "higher")
		{
			correct = (nextCard > card1);
		}
		else
		{
			correct = (nextCard < card1);
		}

		if (correct)
		{
			std::cout << "You guessed right! You win your bet!\n";
			player.addMoney(wager);
		}
		else
		{
			std::cout << "Wrong guess! You lose your bet.\n";
			player.subtractMoney(wager);
		}
	}

	Utils::pressEnterToContinue();
	Utils::clearScreen();
}

void MiniGame::playTreasureHunt(Player &player)
{
	Utils::clearScreen();
	std::cout << "--- MiniGame: Treasure Hunt ---\n";
	std::cout << "Three chests are in front of you. One contains a treasure, and the others are empty.\n";
	std::cout << "Choose a chest (1, 2, or 3): ";

	int chosenChest = 0;
	while (chosenChest < 1 || chosenChest > 3)
	{
		std::cin >> chosenChest;
		if (chosenChest < 1 || chosenChest > 3)
			std::cout << "Invalid choice. Pick a chest (1, 2, or 3): ";
	}

	// Randomly determine which chest contains the treasure
	srand(static_cast<unsigned>(time(nullptr)));
	int treasureChest = rand() % 3 + 1;

	if (chosenChest == treasureChest)
	{
		// Random reward between $500 and $2000
		int reward = (rand() % 1501 + 500);
		std::cout << "Congratulations! You found the treasure and won $" << reward << "!\n";
		player.addMoney(reward);
	}
	else
	{
		int penalty = 500;
		std::cout << "Oops! The treasure was in chest " << treasureChest << ". You lose $" << penalty << ".\n";
		player.subtractMoney(penalty);
	}

	Utils::pressEnterToContinue();
	Utils::clearScreen();
}