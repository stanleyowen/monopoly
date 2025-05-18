#include "Map.h"
#include "GameConfig.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Map::Map() {}

void Map::setTile(int x, int y, char tileSymbol)
{
	board[x][y].setSymbol(tileSymbol);
}

Tile& Map::getTile(int x, int y)
{
	return board[x][y];
}

Tile& Map::getTileById(int tileId)
{
	int x = tileId / 8;
	int y = tileId % 8;
	return board[x][y];
}

void Map::setupBoard()
{
	// Set default to Property tile
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			setTile(i, j, 'P');
		}
	}

	// Empty inner tiles
	for (int i = 1; i < 7; ++i)
	{
		for (int j = 1; j < 7; ++j)
		{
			setTile(i, j, ' ');
		}
	}

	setTile(0, 0, 'S'); // Start
	setTile(0, 2, 'F'); // Fate
	setTile(0, 4, 'I'); // Item Shop
	setTile(3, 7, 'C'); // Chance
	setTile(7, 6, 'H'); // Hospital
	setTile(4, 0, 'C'); // Chance
	setTile(7, 5, 'F'); // Fate
}

void Map::drawBoard(const std::vector<Player>& players) const
{
	// ANSI color codes
	const std::string RESET = "\033[0m";
	const std::string RED_BG = "\033[41m";
	const std::string GREEN_BG = "\033[42m";
	const std::string YELLOW_BG = "\033[43m";
	const std::string MAGENTA_BG = "\033[45m";
	const std::string CYAN_BG = "\033[46m";
	const std::string BLACK_BG = "\033[40m";
	const std::string BOLD = "\033[1m";

	// Access GameConfig instance
	const auto& config = GameConfig::getInstance();
	const auto& propertyLabels = config.getLocationMap();
	const auto& playerSymbols = config.getPlayerIcons();

	auto getTileColor = [&](char sym, const Tile& tile) -> std::string
	{
		if (tile.getOccupied()) // Check if the tile is owned
		{
			for (const auto& player : players)
			{
				if (player.getName() == tile.getOwner())
				{
					return player.getColor(); // Use the owner's color
				}
			}
		}

		switch (sym)
		{
		case 'S':
			return GREEN_BG;
		case 'I':
			return YELLOW_BG;
		case 'C':
			return MAGENTA_BG;
		case 'H':
			return RED_BG;
		case 'F':
			return CYAN_BG;
		default:
			return RESET; // Default background color
		}
	};

	auto getTileLabel = [&](char sym, int tileNumber) -> std::string
	{
		// Fetch the label from the configuration map
		auto it = propertyLabels.find(tileNumber);
		if (it != propertyLabels.end())
		{
			return it->second; // Return the label if found
		}

		// Fallback labels for specific symbols if not found in the configuration
		switch (sym)
		{
		case 'S':
			return "START";
		case 'I':
			return "SHOP";
		case 'C':
			return "CHANCE";
		case 'H':
			return "HOSP";
		case 'F':
			return "FATE";
		case 'P':
			return "PROP";
		default:
			return "????";
		}
	};

	std::cout << BOLD << "+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+" << RESET << "\n";
	for (int i = 0; i < 8; ++i)
	{
		// First row: tile number + label
		for (int j = 0; j < 8; ++j)
		{
			int tileNumber = -1;
			if (i == 0)
				tileNumber = j;
			else if (j == 7)
				tileNumber = 7 + i;
			else if (i == 7)
				tileNumber = 7 + 7 + (7 - j);
			else if (j == 0)
				tileNumber = 7 + 7 + 7 + (7 - i);

			char symbol = board[i][j].getSymbol();
			std::string tileColor = getTileColor(symbol, board[i][j]);
			std::string label = getTileLabel(symbol, tileNumber);

			std::stringstream top;
			if (tileNumber != -1)
			{
				top << std::setw(2) << tileNumber << " " << label;
			}
			else
			{
				top << " ";
			}
			std::cout << "| " << tileColor << std::setw(13) << std::left << top.str() << RESET;
		}
		std::cout << "|\n";

		// Second row: player icons
		for (int j = 0; j < 8; ++j)
		{
			std::stringstream rawIcons;
			for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex)
			{
				const auto& player = players[playerIndex];
				if (player.getX() == i && player.getY() == j)
				{
					// Fetch player symbol and color
					std::string playerSymbol = player.getSymbol();
					std::string color = player.getColor(); // Use the player's color

					// Append the colored symbol
					rawIcons << color << playerSymbol << RESET;
				}
			}

			std::string iconStr = rawIcons.str();
			int visualLength = 0;
			for (size_t k = 0; k < iconStr.size(); ++k)
			{
				if (iconStr[k] == '\033')
				{
					while (k < iconStr.size() && iconStr[k] != 'm')
						++k;
				}
				else
				{
					++visualLength;
				}
			}

			int padLeft = (13 - visualLength) / 2;
			int padRight = 13 - visualLength - padLeft;
			std::cout << "| " << std::string(padLeft, ' ') << iconStr << std::string(padRight, ' ');
		}
		std::cout << "|\n";

		// Third row: property level as 0's
		for (int j = 0; j < 8; ++j)
		{
			std::stringstream levelDisplay;
			const Tile& tile = board[i][j];

			if (tile.getOccupied()) // Check if the property is owned
			{
				int level = tile.getPropertyLevel();	 // Get the property level
				levelDisplay << std::string(level, '0'); // Add '0's based on the level
			}

			int visualLength = levelDisplay.str().size();
			int padLeft = (13 - visualLength) / 2;
			int padRight = 13 - visualLength - padLeft;

			std::cout << "| " << std::string(padLeft, ' ') << levelDisplay.str() << std::string(padRight, ' ');
		}
		std::cout << "|\n";

		std::cout << BOLD << "+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+" << RESET << "\n";
	}

	// === Display Player Info Below the Board ===
	std::cout << "\n";
	std::cout << "+--------------+----------+------------------------------+---------------------+\n";
	std::cout << "| Player Name  |  Assets  | Property                     | Cards               |\n";
	std::cout << "+--------------+----------+------------------------------+---------------------+\n";

	for (const auto& player : players)
	{
		// Cards: convert to comma-separated string
		std::stringstream cardList;
		for (const auto& card : player.getCards())
		{
			cardList << card.getAbbreviatedName() << ",";
		}
		std::string cardsStr = cardList.str();
		if (!cardsStr.empty())
			cardsStr.pop_back(); // Remove trailing comma

		// Properties: convert (x,y) to tile numbers
		std::stringstream propList;
		for (const auto& coord : player.getProperties())
		{
			int x = coord.first;
			int y = coord.second;
			int index = -1;
			if (x == 0)
				index = y;
			else if (y == 7)
				index = 7 + x;
			else if (x == 7)
				index = 7 + 7 + (7 - y);
			else if (y == 0)
				index = 7 + 7 + 7 + (7 - x);
			if (index != -1)
				propList << index << ", ";
		}

		std::string propsStr = propList.str();
		if (!propsStr.empty())
			propsStr.pop_back(), propsStr.pop_back(); // Remove trailing comma

		// Fetch player's symbol and color
		std::string playerSymbol = player.getSymbol();
		std::string color = player.getColor(); // Use the player's color

		// Display player information with colored symbol
		std::cout << "| " << std::left << std::setw(22)
			<< (color + playerSymbol + RESET + " " + player.getName())
			<< "| " << std::right << std::setw(8) << player.getMoney()
			<< " | " << std::left << std::setw(29) << propsStr
			<< "| " << std::left << std::setw(20) << cardsStr << "|\n";
	}

	std::cout << "+--------------+----------+------------------------------+---------------------+\n";
}

void Map::resetProperty(int x, int y)
{
	if (x >= 0 && x < 8 && y >= 0 && y < 8)
	{
		board[x][y].setSymbol('P'); // ��_�����q�a��
		board[x][y].setOccupied(false);
		board[x][y].setOwner("");
		std::cout << "Property at (" << x << ", " << y << ") has been reset.\n";
	}
}

std::vector<Tile>& Map::getTiles()
{
	// Flatten the 2D board to a 1D vector
	static std::vector<Tile> flatTiles;
	flatTiles.clear();
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			flatTiles.push_back(board[i][j]);
		}
	}


	return flatTiles;
}

const std::vector<Tile>& Map::getTiles() const
{
	static std::vector<Tile> flatTiles;
	flatTiles.clear();
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			flatTiles.push_back(board[i][j]);
	return flatTiles;
}