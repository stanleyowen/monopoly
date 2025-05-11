#include "Command.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Game/Map.h"
#include "Card.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

void Command::execute(Game &game, const std::string &input)
{
	std::istringstream iss(input);
	std::string cmd;
	iss >> cmd;

	if (cmd == "/move")
	{
		int playerIndex, destinationId;
		iss >> playerIndex >> destinationId;

		if (playerIndex >= 0 && playerIndex < game.getPlayers().size())
		{
			Player &player = game.getPlayers()[playerIndex];

			// Get the current position ID of the player
			int currentPositionId = player.getPositionId();

			// Calculate the number of steps required to reach the destination
			int totalSteps = (destinationId - currentPositionId + 28) % 28; // Wrap around the board

			player.move(totalSteps); // Move the player to the destination
			game.getMap().drawBoard(game.getPlayers());
			// // Move the player step-by-step
			// for (int i = 0; i < totalSteps; ++i)
			// {
			// 	player.move(1); // Move one step at a time
			// 	game.getMap().drawBoard(game.getPlayers());
			// 	std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Optional: Add delay for animation
			// }

			std::cout << "[Cheat] " << player.getName() << " moved to position " << destinationId << "\n";

			// Handle events after reaching the destination
			game.handleTileEvents(player);
		}
	}
	else if (cmd == "/get")
	{
		int playerIndex, amount;
		iss >> playerIndex >> amount;
		if (playerIndex >= 0 && playerIndex < game.getPlayers().size())
		{
			Player &player = game.getPlayers()[playerIndex];
			player.addMoney(amount);
			std::cout << "[Cheat] " << player.getName() << " received $" << amount << "\n";
		}
	}
	else if (cmd == "/give")
	{
		int from, to, amount;
		iss >> from >> to >> amount;
		auto &players = game.getPlayers();
		if (from >= 0 && from < players.size() && to >= 0 && to < players.size())
		{
			Player &pFrom = players[from];
			Player &pTo = players[to];
			if (pFrom.getMoney() >= amount)
			{
				pFrom.subtractMoney(amount);
				pTo.addMoney(amount);
				std::cout << "[Cheat] $" << amount << " transferred from " << pFrom.getName() << " to " << pTo.getName() << "\n";
			}
			else
			{
				std::cout << "[Cheat] Not enough money to transfer!\n";
			}
		}
	}
	else if (cmd == "/card")
	{
		int playerIndex;
		std::string cardName;
		iss >> playerIndex;
		std::getline(iss, cardName);
		cardName = cardName.substr(cardName.find_first_not_of(" "));
		if (playerIndex >= 0 && playerIndex < game.getPlayers().size())
		{
			Player &player = game.getPlayers()[playerIndex];
			player.addCard(Card(cardName)); // Directly add a Card
			std::cout << "[Cheat] Gave card \"" << cardName << "\" to " << player.getName() << "\n";
		}
	}
	else if (cmd == "/info")
	{
		for (const Player &p : game.getPlayers())
		{
			std::cout << p.getName() << " | $" << p.getMoney()
					  << " | Pos: (" << p.getX() << ", " << p.getY() << ") | Houses: " << p.getHouseCount() << "\n";
		}
	}
	// [!!] Incomplete commands, need to implement
	else if (cmd == "/refresh")
	{
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (cmd == "/gamestate")
	{
		std::cout << "[Cheat] Changed game state (placeholder function).\n";
	}
	else if (cmd == "/minigame")
	{
		std::cout << "[Minigame] Entered a demo minigame (not yet implemented).\n";
	}
	else if (cmd == "/list" || cmd == "/help")
	{
		std::cout << "\n";
		std::cout << "/card       - Retrieve a specific card by name.\n";
		std::cout << "/gamestate  - Change the game state.\n";
		std::cout << "/get        - Get money from the system.\n";
		std::cout << "/give       - Give money to another player.\n";
		std::cout << "/info       - Display information about all players.\n";
		std::cout << "/minigame   - Enter a minigame.\n";
		std::cout << "/move       - Move to a specific position on the board.\n";
		std::cout << "/refresh    - Refresh the game board.\n";
		std::cout << "\n";
	}
	else
	{
		std::cout << "Unknown command.\n";
	}
}
