#include "Command.h"
#include "Game.h"
#include <iostream>
#include <sstream>

void Command::execute(Game& game, const std::string& input) {
	std::istringstream iss(input);
	std::string cmd;
	iss >> cmd;

	if (cmd == "/move") {
		int playerIndex, x, y;
		iss >> playerIndex >> x >> y;
		// Move player manually
	}
	else if (cmd == "/get") {
		int playerIndex, amount;
		iss >> playerIndex >> amount;
		// Give money
	}
	else if (cmd == "/give") {
		int from, to, amount;
		iss >> from >> to >> amount;
		// Transfer money
	}
	else if (cmd == "/card") {
		int playerIndex;
		std::string cardName;
		iss >> playerIndex >> cardName;
		// Play card
	}
	else if (cmd == "/info") {
		// Show player info
	}
	else if (cmd == "/refresh") {
		// Refresh map
	}
	else if (cmd == "/list" || cmd == "/help") {
		std::cout << "Commands: /move, /get, /give, /card, /info, /refresh, /list\n";
	}
	else {
		std::cout << "Unknown command.\n";
	}
}
