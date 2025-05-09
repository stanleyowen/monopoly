#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "GameConfig.h"
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

Game::Game() : currentPlayerIndex(0), gameRunning(true) {
	initializePlayers();
}

void clearScreen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void Game::initializePlayers() {
	players.push_back(Player("Player 1"));  // Adding Player 1
	players.push_back(Player("Player 2"));  // Adding Player 2

	// Random starting cards
	players[0].addCard(Card("Dice Control"));
	players[1].addCard(Card("Special"));
}

std::vector<Player>& Game::getPlayers() {
	return players;
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
	map.drawBoard(players);

	while (gameRunning) {
		processTurn();
	}

	std::cout << "\nGame Over!" << std::endl;
}

void Game::processTurn() {
	// Check that there are players
	if (players.empty()) {
		std::cerr << "No players available!\n";
		return;
	}

	Player& currentPlayer = players[currentPlayerIndex];

	std::cout << currentPlayer.getName() << "'s turn!\n\n";
	std::cout << "Please choose one of the following actions:\n";
	std::cout << "T: Roll the dice\n";
	std::cout << "I: View player info\n";
	std::cout << "(Or type /list for all available commands)\n";

	std::string input;
	std::cout << "> ";

	if (std::cin.peek() == '\n') std::cin.ignore(); // ignore leftover newline
	std::getline(std::cin, input);

	if (input == "T" || input == "t") {
		int diceRoll = rollDice();
		currentPlayer.move(diceRoll);

		handleTileEvents(currentPlayer);
		std::cout << "Rolled: " << diceRoll << std::endl << std::endl;
		checkWinCondition();
	}
	else if (input == "I" || input == "i") {
		currentPlayer.showInfo();
	}
	else if (!input.empty() && input[0] == '/') {
		Command command;
		command.execute(*this, input);
	}
	else {
		std::cout << "Invalid choice. Please select 'T', 'I', or a valid slash command.\n\n";
	}

	// Move to the next player
	currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
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

	clearScreen();
	map.drawBoard(players);
	map.getTile(player.getX(), player.getY()).handleEvent(player);
}

void Game::checkWinCondition() {
	int winMoney = GameConfig::getInstance().getWinMoney();
	int aliveCount = 0;
	int richestIndex = -1;

	for (int i = 0; i < players.size(); ++i) {
		if (players[i].getMoney() >= winMoney) {
			std::cout << "🏆 " << players[i].getName() << " wins with $" << players[i].getMoney() << "!\n";
			gameRunning = false;
			return;
		}
		if (players[i].getMoney() > 0) {
			aliveCount++;
			richestIndex = i;
		}
	}

	if (aliveCount == 1) {
		std::cout << "🏆 " << players[richestIndex].getName() << " wins by survival!\n";
		gameRunning = false;
	}
}
