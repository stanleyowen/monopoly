#include "GameConfig.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

GameConfig GameConfig::instance;

// Singleton accessor
GameConfig &GameConfig::getInstance()
{
	return instance;
}

// Load configuration from JSON file
void GameConfig::loadConfig()
{
	std::string configPath = "json/config.json";
	std::string dialoguePath = "json/dialogue.json";
	std::ifstream fileConfig(configPath);
	std::ifstream fileDialogue(dialoguePath);

	if (!fileDialogue.is_open())
	{
		std::cerr << "Cannot open dialogue file: " << dialoguePath << "\n";
		return;
	}
	if (!fileConfig.is_open())
	{
		std::cerr << "Cannot open config file: " << configPath << "\n";
		return;
	}

	nlohmann::json config, dialogue;
	fileConfig >> config;
	fileDialogue >> dialogue;

	// If the mode is release, ask the user to input the number of players and the player names
	// If the mode is not release, read all the value from the specified mode in config.json
	std::string mode = config["defaultMode"].get<std::string>();
	if (mode == "RELEASE")
	{
		std::cout << dialogue["input_player_num"].get<std::string>() << "\n";
		std::cin >> playersNum;

		playerNames.resize(playersNum);
		for (int i = 0; i < playersNum; ++i)
		{
			std::cout << dialogue["input_player_name"].get<std::string>() << " " << i + 1 << ": ";
			std::getline(std::cin, playerNames[i]);
		}
	}
	else
	{
		// Parse the built-in player-related configurations
		playersNum = config["modes"][mode]["playersNum"].get<int>();
		playerNames = config["modes"][mode]["playersName"].get<std::vector<std::string>>();
	}

	// Parse game mode configurations
	startMoney = config["modes"][mode]["startMoney"].get<int>();
	winMoney = config["modes"][mode]["winMoney"].get<int>();
	passingStartBonus = config["modes"][mode]["passingStartBonus"].get<int>();

	// Parse default player-related configurations
	playerIcons = config["playerIcons"].get<std::vector<std::string>>();
	playerColors = config["playerColors"].get<std::vector<std::string>>();

	mapSize = config["mapSize"].get<int>();
	tileWidth = config["tileWidth"].get<size_t>();
	animation = config["animation"].get<bool>();
	animationTime = config["animationTime"].get<long long>();
	propertyLevelIcons = config["propertyLevelIcons"].get<std::vector<std::string>>();

	// Parse the board tiles
	for (const auto &tile : config["boardTiles"])
	{
		TileConfig tileConfig;
		tileConfig.id = tile["index"].get<std::size_t>();
		tileConfig.type = tile["type"].get<std::string>();
		tileConfig.name = tile["name"].get<std::string>();
		tileConfig.price = tile["price"].get<int>();
		tileConfig.toll = tile["toll"].get<int>();
		boardTiles.push_back(tileConfig);
		locationMap[tileConfig.id] = tileConfig.name;
	}

	// Parse the cards
	for (const auto &card : config["cards"])
	{
		CardConfig cardConfig;
		cardConfig.name = card["name"].get<std::string>();
		cardConfig.icon = card["icon"].get<std::string>();
		cardConfig.effect = card["effect"].get<std::string>();
		cardConfig.price = card["price"].get<int>();
		cards.push_back(cardConfig);
	}

	// Parse eventValueRange
	for (const auto &event : config["eventValueRange"].items())
	{
		std::string eventName = event.key();
		int minValue = event.value()[0].get<int>(),
			maxValue = event.value()[1].get<int>();
		eventValueRange[eventName] = std::make_pair(minValue, maxValue);
	}
}

// Setters and getters for GameConfig properties
void GameConfig::setMode(GameMode newMode) { mode = newMode; }
GameMode GameConfig::getMode() const { return mode; }

void GameConfig::setPlayersNum(int num) { playersNum = num; }
int GameConfig::getPlayersNum() const { return playersNum; }

void GameConfig::setPlayersName(const std::vector<std::string> &names) { playerNames = names; }
std::vector<std::string> GameConfig::getPlayerNames() const { return playerNames; }

void GameConfig::setPlayerIcons(const std::vector<std::string> &icons) { playerIcons = icons; }
std::vector<std::string> GameConfig::getPlayerIcons() const { return playerIcons; }

void GameConfig::setPlayerColors(const std::vector<std::string> &colors) { playerColors = colors; }
std::vector<std::string> GameConfig::getPlayerColors() const { return playerColors; }

void GameConfig::setPropertyLevelIcons(const std::vector<std::string> &icons) { propertyLevelIcons = icons; }
std::vector<std::string> GameConfig::getPropertyLevelIcons() const { return propertyLevelIcons; }

std::map<int, std::string> GameConfig::getLocationMap() const { return locationMap; }

void GameConfig::setStartMoney(int amount) { startMoney = amount; }
int GameConfig::getStartMoney() const { return startMoney; }

void GameConfig::setWinMoney(int amount) { winMoney = amount; }
int GameConfig::getWinMoney() const { return winMoney; }

void GameConfig::setPassingStartBonus(int amount) { passingStartBonus = amount; }
int GameConfig::getPassingStartBonus() const { return passingStartBonus; }

void GameConfig::setBoardTiles(const std::vector<TileConfig> &tiles) { boardTiles = tiles; }
std::vector<TileConfig> GameConfig::getBoardTiles() const { return boardTiles; }

void GameConfig::setCards(const std::vector<CardConfig> &cards)
{
	// cards = cards;
}
std::vector<CardConfig> GameConfig::getCards() const { return cards; }

void GameConfig::setEventValueRange(const std::map<std::string, std::pair<int, int>> &range) { eventValueRange = range; }
std::map<std::string, std::pair<int, int>> GameConfig::getEventValueRange() const { return eventValueRange; }

void GameConfig::setAnimation(bool status) { animation = status; }
bool GameConfig::getAnimation() const { return animation; }

int GameConfig::getMapSize() const { return mapSize; }

int GameConfig::getTileWidth() const { return tileWidth; }

long long GameConfig::getAnimationTime() const { return animationTime; }