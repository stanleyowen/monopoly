#pragma once
#include "Player.h"
#include "Map.h"
#include "Command.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

class Game
{
public:
	Game();
	Map &getMap() { return map; }
	void initializePlayers();
	std::vector<Player> &getPlayers();
	void start();
	void processTurn();

	int rollDice();
	void handleTileEvents(Player &player);
	void checkWinCondition();

private:
	Map map;
	std::vector<Player> players;
	int currentPlayerIndex;
	bool gameRunning;
};

void clearScreen();
