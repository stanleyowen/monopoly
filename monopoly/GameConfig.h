#pragma once
#include <string>
#include <vector>
#include <map>

class GameConfig {
public:
	static GameConfig& getInstance();

	bool loadConfig(const std::string& filename = "config.json");

	int getStartMoney() const;
	int getWinMoney() const;
	int getPassingStartBonus() const;
	int getMapSize() const;

private:
	GameConfig() = default;
	GameConfig(const GameConfig&) = delete;
	GameConfig& operator=(const GameConfig&) = delete;

	int startMoney = 5000;
	int winMoney = 20000;
	int passingStartBonus = 2000;
	int mapSize = 32;
};
