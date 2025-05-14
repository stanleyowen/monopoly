#pragma once
#include "Player.h"
#include "Map.h"
#include "Command.h"
#include "Utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

class Game
{
public:
	Game();
	Map& getMap() { return map; }
	void initializePlayers();
	std::vector<Player>& getPlayers();
	void start();
	void animatePlayerMovement(Player& player, int steps, int dice1, int dice2);
	void displayDiceAnimation(int dice1, int dice2, const std::vector<Player>& players);
	void processTurn();

	void handleTileEvents(Player& player);
	void checkWinCondition();
	int getCurrentPlayerIndex() const;
	int getTileIdByName(const std::string& name) const;



private:
	Map map;
	std::vector<Player> players;
	int currentPlayerIndex;
	bool gameRunning;

};
