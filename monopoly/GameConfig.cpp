#include "GameConfig.h"
#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

GameConfig& GameConfig::getInstance() {
	static GameConfig instance;
	return instance;
}

bool GameConfig::loadConfig(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Cannot open config file: " << filename << "\n";
		return false;
	}

	json j;
	file >> j;

	startMoney = j.value("startMoney", 5000);
	winMoney = j.value("winMoney", 20000);
	passingStartBonus = j.value("passingStartBonus", 2000);
	mapSize = j.value("mapSize", 32);

	return true;
}

int GameConfig::getStartMoney() const { return startMoney; }
int GameConfig::getWinMoney() const { return winMoney; }
int GameConfig::getPassingStartBonus() const { return passingStartBonus; }
int GameConfig::getMapSize() const { return mapSize; }
