#include "Command.h"
#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Card.h"
#include <iostream>
#include <sstream>

void Command::execute(Game& game, const std::string& input) {
	std::istringstream iss(input);
	std::string cmd;
	iss >> cmd;

	if (cmd == "/move") {
		int playerIndex, x, y;
		iss >> playerIndex >> x >> y;
		if (playerIndex >= 0 && playerIndex < game.getPlayers().size()) {
			Player& player = game.getPlayers()[playerIndex];
			player.setPosition(x, y);
			std::cout << "[Cheat] " << player.getName() << " moved to (" << x << ", " << y << ")\n";
		}
	}
	else if (cmd == "/get") {
		int playerIndex, amount;
		iss >> playerIndex >> amount;
		if (playerIndex >= 0 && playerIndex < game.getPlayers().size()) {
			Player& player = game.getPlayers()[playerIndex];
			player.addMoney(amount);
			std::cout << "[Cheat] " << player.getName() << " received $" << amount << "\n";
		}
	}
	else if (cmd == "/give") {
		int from, to, amount;
		iss >> from >> to >> amount;
		auto& players = game.getPlayers();
		if (from >= 0 && from < players.size() && to >= 0 && to < players.size()) {
			Player& pFrom = players[from];
			Player& pTo = players[to];
			if (pFrom.getMoney() >= amount) {
				pFrom.subtractMoney(amount);
				pTo.addMoney(amount);
				std::cout << "[Cheat] $" << amount << " transferred from " << pFrom.getName() << " to " << pTo.getName() << "\n";
			}
			else {
				std::cout << "[Cheat] Not enough money to transfer!\n";
			}
		}
	}
	else if (cmd == "/card") {
		int playerIndex;
		std::string cardName;
		iss >> playerIndex;
		std::getline(iss, cardName);
		cardName = cardName.substr(cardName.find_first_not_of(" "));
		if (playerIndex >= 0 && playerIndex < game.getPlayers().size()) {
			Player& player = game.getPlayers()[playerIndex];
			player.addCard(Card(cardName));  // Directly add a Card
			std::cout << "[Cheat] Gave card \"" << cardName << "\" to " << player.getName() << "\n";
		}
	}
	else if (cmd == "/info") {
		for (const Player& p : game.getPlayers()) {
			std::cout << p.getName() << " | $" << p.getMoney()
				<< " | Pos: (" << p.getX() << ", " << p.getY() << ") | Houses: " << p.getHouseCount() << "\n";
		}
	}
	else if (cmd == "/list" || cmd == "/help") {
		std::cout << "Commands: /move <i> <x> <y>, /get <i> <amount>, /give <from> <to> <amount>, /card <i> <name>, /info, /list\n";
	}
	else {
		std::cout << "Unknown command.\n";
	}
}
