#include "Game.h"
#include "Player.h"
#include "Map.h"
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

Game::Game() : currentPlayerIndex(0), gameRunning(true) {
	initializePlayers();
}

void Game::initializePlayers() {
	players.push_back(Player("Player 1"));  // Adding Player 1
	players.push_back(Player("Player 2"));  // Adding Player 2

	// Random starting cards
	players[0].addCard(Card("Dice Control"));
	players[1].addCard(Card("Special"));
}

void Game::start() {
	std::cout << "Welcome to Monopoly!\n\n";
	std::cout << "Game starting...\n\n";

	// Add intro animation and other processes after this
	for (int i = 3; i > 0; --i) {
		std::cout << "Game starts in " << i << "...\n";
	}

	std::cout << "\nStart!\n";

	map.setupBoard();

	while (gameRunning) {
		processTurn();
	}

	std::cout << "\nGame Over!" << std::endl;
}

void Game::processTurn() {
	// Check that there are players
	if (players.size() > 0) {
		Player& currentPlayer = players[currentPlayerIndex];

		std::cout << currentPlayer.getName() << "'s turn!\n\n";

		std::cout << "Please choose an action:" << std::endl;
		std::cout << "T: Roll the dice." << std::endl;
		std::cout << "I: View player info." << std::endl;

		char action;
		std::cout << "> ";
		std::cin >> action;

		if (action == 'T' || action == 't') {
			int diceRoll = rollDice();
			std::cout << "Rolled: " << diceRoll << std::endl;

			currentPlayer.move(diceRoll);
			handleTileEvents(currentPlayer);
			checkWinCondition();
		}
		else if (action == 'I' || action == 'i') {
			currentPlayer.showInfo();

			// Can add more info here later
		}
		else {
			std::cout << "Invalid choice. Please select 'T' to roll the dice or 'I' for player info.\n\n";
		}

		// Move to the next player
		currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
	}
	else {
		std::cerr << "No players available!\n";
	}
}

int Game::rollDice() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(1, 6);
	return dis(gen) + dis(gen);
}

void Game::handleTileEvents(Player& player) {
	// Get the player's position
	int x = player.getX();
	int y = player.getY();

	Tile& currentTile = map.getTile(x, y);

	currentTile.handleEvent(player);
}

void Game::checkWinCondition() {
	for (const auto& player : players) {
		// Example win condition
		if (player.getMoney() >= 10000) {
			std::cout << player.getName() << " has won the game!\n";
			gameRunning = false;  // End the game
			break;
		}
	}
}
