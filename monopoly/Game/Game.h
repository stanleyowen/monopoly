#pragma once
#include "Player.h"
#include "Map.h"
#include "../Command.h"
#include "../Utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

enum class GameState
{
	INIT,
	START,
	MOVED,
	ROUND_END,
	FINISH
};

class Game
{
public:
	Game();
	static Game& getInstance(); // ��ҼҦ���k
	Map& getMap();
	void initializePlayers();
	std::vector<Player>& getPlayers();
	void start();
	void animatePlayerMovement(Player& player, int steps, int dice1, int dice2);
	void animateControlledPlayerMovement(Player& player, int steps, int diceValue);
	void displayDiceAnimation(int dice1, int dice2, const std::vector<Player>& players);
	void processTurn();

	void handleTileEvents(Player& player);
	void checkWinCondition();
	void displayGameOver();

	int getCurrentPlayerIndex() const;
	void setCurrentPlayerIndex(int index);
	int getTileIdByName(const std::string& name) const;

	void setGameState(GameState state);
	GameState getGameState() const;

	bool saveGame(const std::string& filename);
	bool loadGame(const std::string& filename);

private:
	Map map;
	std::vector<Player> players;
	int currentPlayerIndex;
	bool gameRunning;

	GameState currentState = GameState::INIT;

	Game(const Game&) = delete;			// �R�������c�y
	Game& operator=(const Game&) = delete; // �R����ȹB��
	static Game instance;					// ��ҹ��
};
