#pragma once

#include <map>
#include <string>
#include <vector>

enum class GameMode
{
	DEBUG,
	DUEL,
	RELEASE,
	CUSTOM
};

struct TileConfig
{
	std::size_t id;
	std::string type;
	std::string name;

	int price;
	int toll;
};

struct CardConfig
{
	std::string name;
	std::string icon;
	std::string effect;

	int price;
};

class GameConfig
{
private:
	static GameConfig instance;
	GameMode mode = GameMode::DEBUG;

	int playersNum = 2;
	int startMoney = 0;
	int winMoney = 0;
	int passingStartBonus = 0;
	int mapSize = 8;
	size_t tileWidth = 14;

	bool animation = true;
	long long animationTime = 0;

	std::vector<std::string> playerNames;
	std::vector<std::string> playerIcons;
	std::vector<std::string> playerColors;

	std::vector<TileConfig> boardTiles;
	std::vector<CardConfig> cards;
	std::vector<std::string> propertyLevelIcons;
	std::map<int, std::string> locationMap;
	std::map<std::string, std::pair<int, int>> eventValueRange;

	GameConfig() = default;

public:
	static GameConfig &getInstance();

	// Set and get game mode
	void setMode(GameMode newMode);
	GameMode getMode() const;

	// Set and get number of players
	void setPlayersNum(int num);
	int getPlayersNum() const;

	// Set and get player names
	void setPlayersName(const std::vector<std::string> &names);
	std::vector<std::string> getPlayerNames() const;

	// Set and get player icons
	void setPlayerIcons(const std::vector<std::string> &icons);
	std::vector<std::string> getPlayerIcons() const;

	// Set and get player colors
	void setPlayerColors(const std::vector<std::string> &colors);
	std::vector<std::string> getPlayerColors() const;

	// Set and get property level icons
	void setPropertyLevelIcons(const std::vector<std::string> &icons);
	std::vector<std::string> getPropertyLevelIcons() const;

	std::map<int, std::string> getLocationMap() const;

	// Set and get money information
	void setStartMoney(int amount);
	int getStartMoney() const;

	void setWinMoney(int amount);
	int getWinMoney() const;

	void setPassingStartBonus(int amount);
	int getPassingStartBonus() const;

	// Set and get board tile information
	void setBoardTiles(const std::vector<TileConfig> &tiles);
	std::vector<TileConfig> getBoardTiles() const;

	// Set and get cards information
	void setCards(const std::vector<CardConfig> &cards);
	std::vector<CardConfig> getCards() const;

	// Set and get event value range
	void setEventValueRange(const std::map<std::string, std::pair<int, int>> &range);
	std::map<std::string, std::pair<int, int>> getEventValueRange() const;

	// Set and get animation status
	void setAnimation(bool status);
	bool getAnimation() const;

	int getMapSize() const;

	int getTileWidth() const;

	long long getAnimationTime() const;

	void loadConfig();
};
