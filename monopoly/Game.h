#pragma once
#include "Player.h"
#include "Map.h"
#include "Command.h"

class Game {
public:
	Game();
	Map& getMap() { return map; }
	void initializePlayers();
	std::vector<Player>& getPlayers();
	void start();
	void processTurn();

	int rollDice();
	void handleTileEvents(Player& player);
	void checkWinCondition();

private:
	Map map;
	std::vector<std::vector<Tile>> board;
	std::vector<Player> players;
	int currentPlayerIndex;
	bool gameRunning;
};
