#include "Command.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Game/Map.h"
#include "Card.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

void Command::execute(Game& game, const std::string& input)
{
	std::istringstream iss(input);
	std::string cmd;
	iss >> cmd;

	if (cmd == "/move")
	{
		std::string subCmd;
		iss >> subCmd;

		int destinationId = -1;
		if (subCmd == "to")
		{
			std::string destinationName;
			std::getline(iss, destinationName);
			destinationName = destinationName.substr(destinationName.find_first_not_of(" ")); // 去除空格

			// 使用 Game 物件來查詢地名對應的 ID
			destinationId = game.getTileIdByName(destinationName);
			if (destinationId == -1)
			{
				std::cout << "[錯誤] 找不到地名：" << destinationName << "\n";
				return;
			}
		}
		else {
			// 嘗試將子指令轉換為數字 ID
			try
			{
				destinationId = std::stoi(subCmd);
			}
			catch (const std::invalid_argument&)
			{
				std::cout << "[錯誤] 無效的指令格式：" << subCmd << "\n";
				return;
			}
		}

		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player& player = game.getPlayers()[currentPlayerIndex];

		// Get the current position ID of the player
		int currentPositionId = player.getPositionId();

		// Calculate the number of steps required to reach the destination
		int totalSteps = (destinationId - currentPositionId + 28) % 28; // Wrap around the board

		player.move(totalSteps); // Move the player to the destination
		game.getMap().drawBoard(game.getPlayers());
		// // Move the player step-by-step
		// for (int i = 0; i < totalSteps; ++i)
		// {
		// 	player.move(1); // Move one step at a time
		// 	game.getMap().drawBoard(game.getPlayers());
		// 	std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Optional: Add delay for animation
		// }

		std::cout << "[Cheat] " << player.getName() << " moved to position " << destinationId << "\n";

		// Handle events after reaching the destination
		game.handleTileEvents(player);

	}
	else if (cmd == "/get")
	{
		std::string targetName;
		int amount;

		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player& currentPlayer = game.getPlayers()[currentPlayerIndex];

		// 嘗試讀取金額或玩家名稱
		if (!(iss >> amount))
		{
			// 如果第一個參數不是數字，則表示有指定玩家
			iss.clear(); // 清除錯誤狀態
			iss >> targetName >> amount;

			// 查找指定名字的玩家
			int targetPlayerIndex = -1;
			for (size_t i = 0; i < game.getPlayers().size(); ++i)
			{
				if (game.getPlayers()[i].getName() == targetName)
				{
					targetPlayerIndex = i;
					break;
				}
			}

			// 如果找不到該玩家
			if (targetPlayerIndex == -1)
			{
				std::cout << "[錯誤] 玩家 " << targetName << " 不存在。\n";
				return;
			}

			Player& targetPlayer = game.getPlayers()[targetPlayerIndex];
			targetPlayer.addMoney(amount);
			std::cout << "[作弊] " << targetPlayer.getName() << " 從系統取得 $" << amount << "\n";
		}
		else
		{
			// 沒有指定玩家，直接給當前玩家
			currentPlayer.addMoney(amount);
			std::cout << "[作弊] " << currentPlayer.getName() << " 從系統取得 $" << amount << "\n";
		}

		// 更新遊戲畫面
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (cmd == "/give")
	{
		/*int from, to, amount;
		iss >> from >> to >> amount;
		auto& players = game.getPlayers();
		if (from >= 0 && from < players.size() && to >= 0 && to < players.size())
		{
			Player& pFrom = players[from];
			Player& pTo = players[to];
			if (pFrom.getMoney() >= amount)
			{
				pFrom.subtractMoney(amount);
				pTo.addMoney(amount);
				std::cout << "[Cheat] $" << amount << " transferred from " << pFrom.getName() << " to " << pTo.getName() << "\n";
			}
			else
			{
				std::cout << "[Cheat] Not enough money to transfer!\n";
			}
		}*/

		std::string targetName;
		int amount;
		iss >> targetName >> amount;

		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player& currentPlayer = game.getPlayers()[currentPlayerIndex];

		// 查找指定名字的玩家
		int targetPlayerIndex = -1;
		for (size_t i = 0; i < game.getPlayers().size(); ++i)
		{
			if (game.getPlayers()[i].getName() == targetName)
			{
				targetPlayerIndex = i;
				break;
			}
		}

		// 如果找不到該玩家
		if (targetPlayerIndex == -1)
		{
			std::cout << "[錯誤] 玩家 " << targetName << " 不存在。\n";
			return;
		}

		Player& targetPlayer = game.getPlayers()[targetPlayerIndex];

		// 檢查自己是否有足夠的金錢
		if (currentPlayer.getMoney() >= amount)
		{
			currentPlayer.subtractMoney(amount);
			targetPlayer.addMoney(amount);
			std::cout << "[交易] " << currentPlayer.getName() << " 給予 " << targetPlayer.getName() << " $" << amount << "\n";
			// 立即更新遊戲畫面
			game.getMap().drawBoard(game.getPlayers());

		}
		else
		{
			std::cout << "[錯誤] " << currentPlayer.getName() << " 金錢不足。\n";
		}

	}
	else if (cmd == "/card")
	{
		std::string cardName;
		std::getline(iss, cardName);
		cardName = cardName.substr(cardName.find_first_not_of(" ")); // 去除多餘空格

		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player& currentPlayer = game.getPlayers()[currentPlayerIndex];

		if (cardName.empty())
		{
			std::cout << "[錯誤] 卡牌名稱不可為空。\n";
			return;
		}

		// 將指定名稱的卡牌加入玩家卡牌列表
		currentPlayer.addCard(Card(cardName));
		std::cout << "[作弊] " << currentPlayer.getName() << " 獲得卡牌：" << cardName << "\n";

		// 更新遊戲畫面
		game.getMap().drawBoard(game.getPlayers());
		return; // 不切換回合

	}
	else if (cmd == "/info")
	{
		for (const Player& p : game.getPlayers())
		{
			std::cout << p.getName() << " | $" << p.getMoney()
				<< " | Pos: (" << p.getX() << ", " << p.getY() << ") | Houses: " << p.getHouseCount() << "\n";
		}
	}
	// [!!] Incomplete commands, need to implement
	else if (cmd == "/refresh")
	{
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (cmd == "/gamestate")
	{
		std::cout << "[Cheat] Changed game state (placeholder function).\n";
	}
	else if (cmd == "/minigame")
	{
		std::cout << "[Minigame] Entered a demo minigame (not yet implemented).\n";
	}
	else if (cmd == "/list" || cmd == "/help")
	{
		std::cout << "\n";
		std::cout << "/card       - Retrieve a specific card by name.\n";
		std::cout << "/gamestate  - Change the game state.\n";
		std::cout << "/get        - Get money from the system.\n";
		std::cout << "/give       - Give money to another player.\n";
		std::cout << "/info       - Display information about all players.\n";
		std::cout << "/minigame   - Enter a minigame.\n";
		std::cout << "/move       - Move to a specific position on the board.\n";
		std::cout << "/refresh    - Refresh the game board.\n";
		std::cout << "\n";
	}
	else
	{
		std::cout << "Unknown command.\n";
	}
}
