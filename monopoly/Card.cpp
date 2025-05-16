#include "Card.h"
#include "Game/Map.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Tile.h"
#include "Game/GameConfig.h"
#include <iostream>

Card::Card(const std::string& type) : type(type) {}

std::string Card::getType() const {
	return type;
}

void Card::applyEffect(Player& player, std::vector<Player>& players, Map& map)
{
	if (type == "Dice Card") //����d
	{
		int diceValue;
		std::cout << "Choose your dice value (1-12): ";
		std::cin >> diceValue;
		std::cin.ignore();

		if (diceValue >= 1 && diceValue <= 12)
		{
			player.setNextDiceValue(diceValue);
			std::cout << "Dice Control Card used! Next dice will roll: " << diceValue << "\n";
			player.removeCard("Dice Card");
		}
		else
		{
			std::cout << "Invalid dice value. Please enter a number between 1 and 12.\n";
		}
	}
	else if (type == "Barrier Card") { //���٥d
		std::cout << "Placing a barrier on a tile to block players.\n";
		// Implement barrier logic here
	}
	else if (type == "Destroy Card") { //��d
		//std::cout << "Destroying another player's property.\n";
		// Implement property destruction logic here
		std::cout << "Choose a player to destroy their property:\n";

		// �C�X�Ҧ����a
		for (size_t i = 0; i < players.size(); ++i)
		{
			std::cout << i + 1 << ". " << players[i].getName() << "\n";
		}

		int playerChoice;
		std::cout << "Enter player number (0 to cancel): ";
		std::cin >> playerChoice;
		std::cin.ignore();

		if (playerChoice <= 0 || playerChoice > players.size())
		{
			std::cout << "Invalid choice. Canceling destroy action.\n";
			return;
		}

		Player& targetPlayer = players[playerChoice - 1];
		const auto& properties = targetPlayer.getProperties();

		// �ˬd���a�O�_���в�
		if (properties.empty())
		{
			std::cout << targetPlayer.getName() << " has no properties to destroy.\n";
			return;
		}

		// �C�X�ؼЪ��a���Ҧ��в�
		std::cout << "Choose a property to destroy:\n";
		for (size_t i = 0; i < properties.size(); ++i)
		{
			// ���T����в���m (x, y)
			int x = properties[i].first;
			int y = properties[i].second;

			// �p�� tileId (���]�a�Ϭ� 8x8)
			int tileId = -1;

			if (x == 0)
				tileId = y;
			else if (y == 7)
				tileId = 7 + x;
			else if (x == 7)
				tileId = 7 + 7 + (7 - y);
			else if (y == 0)
				tileId = 7 + 7 + 7 + (7 - x);

			GameConfig& config = GameConfig::getInstance();
			auto locationMap = config.getLocationMap();

			// ����a�W
			std::string propertyName = (locationMap.find(tileId) != locationMap.end()) ? locationMap[tileId] : "Unknown";

			//Game& game = Game::getInstance();
			//Map& map = game.getMap();
			Tile& tile = map.getTile(x, y);
			std::cout << tile.getSymbol() << "\n";
			int buildingLevel = tile.getPropertyLevel();

			std::cout << i + 1 << ". " << propertyName << " (Level " << buildingLevel << ")\n";
		}

		int propertyChoice;
		std::cout << "Enter property number (0 to cancel): ";
		std::cin >> propertyChoice;
		std::cin.ignore();

		if (propertyChoice <= 0 || propertyChoice > properties.size())
		{
			std::cout << "Invalid choice. Canceling destroy action.\n";
			return;
		}

		// ����襤���в��y��
		int x = properties[propertyChoice - 1].first;
		int y = properties[propertyChoice - 1].second;

		// �p�� tileId
		int tileId = -1;

		if (x == 0)
			tileId = y;
		else if (y == 7)
			tileId = 7 + x;
		else if (x == 7)
			tileId = 7 + 7 + (7 - y);
		else if (y == 0)
			tileId = 7 + 7 + 7 + (7 - x);

		//Game& game = Game::getInstance();
		//Map& map = game.getMap();
		Tile& tile = map.getTile(x, y);
		std::cout << tile.getSymbol() << "\n";
		GameConfig& config = GameConfig::getInstance();
		auto locationMap = config.getLocationMap();
		std::string propertyName = (locationMap.find(tileId) != locationMap.end()) ? locationMap[tileId] : "Unknown";



		if (tile.getPropertyLevel() > 1)
		{
			tile.setPropertyLevel(tile.getPropertyLevel() - 1);
			std::cout << "Property \"" << propertyName << "\" downgraded to Level " << tile.getPropertyLevel() << "!\n";
		}
		else if (tile.getPropertyLevel() == 1)
		{
			tile.setPropertyLevel(tile.getPropertyLevel() - 1);
			tile.setOccupied(false);
			tile.setOwner("");
			//targetPlayer.removeProperty(x, y);
			//tile.setPropertyLevel(0);

			std::cout << "Property \"" << propertyName << "\" destroyed!\n";
		}
		player.removeCard("Destroy Card");
	}
	else
	{
		std::cout << "Unknown or unsupported card type.\n";
	}
}



/* void Card::applyEffect(Player& player, std::vector<std::shared_ptr<Player>>& players) {
	if (type == "Barrier Card") { //���٥d
		std::cout << "Placing a barrier on a tile to block players.\n";
		// Implement barrier logic here
	}
	else if (type == "Dice Card") { //�뱱�d
		std::cout << "Choose the number you roll on the dice.\n";
		// Implement dice control logic here
		int diceValue;
		std::cout << "Choose your dice value (1-12): ";
		std::cin >> diceValue;

		if (diceValue >= 1 && diceValue <= 12) {
			player.setNextDiceValue(diceValue);
			std::cout << "Dice Control Card used! Next dice will roll: " << diceValue << "\n";
			player.removeCard("Dice Card");
		}
		else {
			std::cout << "Invalid dice value. Please enter a number between 1 and 12.\n";
		}
	}
	else if (type == "Destroy Card") { //��d
		std::cout << "Destroying another player's property.\n";
		// Implement property destruction logic here

	}
	else if (type == "Fate Card") { //�R�B�d
		std::cout << "Triggering a Fate event.\n";
		// Implement fate event logic here
		int fate = rand() % 2;
		if (fate == 0) {
			std::cout << "You gained $500.\n";
			player.addMoney(500);
		}
		else {
			std::cout << "You lost $300.\n";
			player.subtractMoney(300);
		}
	}
	else if (type == "Rocket Card") { //���b�d
		std::cout << "Sending a player to the hospital for 2 turns.\n";
		// Implement rocket card logic here

	}
	else {
		std::cout << "Unknown card type.\n";
	}
}*/
