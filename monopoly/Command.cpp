#include "Command.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Game/Map.h"
#include "MiniGame.h"
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

		int currentPositionId = player.getPositionId();
		int totalSteps = (destinationId - currentPositionId + 28) % 28; // Wrap around the board

		 // Move the player to the destination
		player.move(totalSteps);

		game.getMap().drawBoard(game.getPlayers());
		// Move the player step-by-step
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
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
		for (const Player& p : game.getPlayers())
		{
			p.showInfo();
		}
		/*// 獲取遊戲單例
		const std::vector<Player>& players = game.getPlayers(); // 獲取所有玩家
		std::cout << "玩家數量: " << players.size() << std::endl; // 打印玩家數量

		if (players.empty())
		{
			std::cerr << "No players available!\n";
			return;
		}
		for (const Player& player : players)
		{
			player.showInfo();    // 呼叫每個玩家的 showInfo() 函式
			std::cout << std::endl; // 分隔不同玩家資訊
		}


	}
	else if (cmd == "/refresh")
	{
		game.getMap().drawBoard(game.getPlayers());
		std::cout << "[系統] 地圖已重新繪製。\n";
		return; // 不切換回合
	}
	else if (cmd == "/gamestate")
	{
		std::string stateStr;
		iss >> stateStr;

		// Convert string to GameState
		std::transform(stateStr.begin(), stateStr.end(), stateStr.begin(), ::toupper);

		GameState newState;

		if (stateStr == "INIT") {
			newState = GameState::INIT;
		}
		else if (stateStr == "START") {
			newState = GameState::START;
		}
		else if (stateStr == "MOVED") {
			newState = GameState::MOVED;
		}
		else if (stateStr == "ROUND_END") {
			newState = GameState::ROUND_END;
		}
		else if (stateStr == "FINISH") {
			newState = GameState::FINISH;
		}
		else {
			std::cout << "Invalid input. Please enter: INIT, START, MOVED, ROUND_END, FINISH\n";
			return;
		}

		game.setGameState(newState);
		std::cout << "[Cheat] Changed the game state into " << stateStr << "\n";
	}
	else if (cmd == "/minigame")
	{
		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player& currentPlayer = game.getPlayers()[currentPlayerIndex];

		while (true)
		{
			int choice;

			// 顯示小遊戲選單
			Utils::clearScreen();
			std::cout << "\n======================== MiniGame Menu ========================\n";
			std::cout << "1. Horse Racing Game\n";
			std::cout << "2. Dragon Gate Game\n";
			std::cout << "3. Treasure Hunt Game\n";
			std::cout << "4. Maze Escape Game\n";
			std::cout << "5. Return\n";
			std::cout << "===============================================================\n";
			std::cout << "Please choose (1~5): ";

			std::cin >> choice;
			std::cin.ignore();

			switch (choice)
			{
			case 1:
				std::cout << "\n[小遊戲] 進入 Horse Racing Game！\n";
				MiniGame::playHorseRace(currentPlayer);
				break;
			case 2:
				std::cout << "\n[小遊戲] 進入 Dragon Gate Game！\n";
				MiniGame::playDragonGate(currentPlayer);
				break;
			case 3:
				std::cout << "\n[小遊戲] 進入 Treasure Hunt Game！\n";
				MiniGame::playTreasureHunt(currentPlayer);
				break;
			case 4:
				std::cout << "\n[小遊戲] 進入 Maze Escape Game！\n";
				MiniGame::playMazeEscape(currentPlayer);
				break;
			case 5:
				std::cout << "[返回] 回到主畫面。\n";
				game.getMap().drawBoard(game.getPlayers());

				break;
			default:
				std::cout << "[錯誤] 無效的選擇，請輸入 1~5。\n";
				Utils::pressEnterToContinue();
				break;
			}

			// 更新地圖顯示
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
			return;
		}
	}
	else if (cmd == "/list" || cmd == "/help")
	{
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
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

	//Utils::clearScreen();
	//game.getMap().drawBoard(game.getPlayers());
	//game.checkWinCondition();
}