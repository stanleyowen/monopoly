﻿#include "Command.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Game/Map.h"
#include "Game/GameConfig.h"
#include "MiniGame.h"
#include "Card.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

nlohmann::json Command::commandData;

void Command::loadCommandData()
{
	std::string commandPath = "json/command.json";
	std::ifstream fileCommand(commandPath);

	if (!fileCommand.is_open())
	{
		std::cerr << "Cannot open command file: " << commandPath << "\n";
		return;
	}

	fileCommand >> commandData;
}

void Command::execute(Game &game, const std::string &input)
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
				std::cout << "[Error] Property not found：" << destinationName << "\n";
				return;
			}
		}
		else
		{
			// 嘗試將子指令轉換為數字 ID
			try
			{
				destinationId = std::stoi(subCmd);
			}
			catch (const std::invalid_argument &)
			{
				std::cout << "[Error] Invalid instruction format：" << subCmd << "\n";
				return;
			}
		}

		if (destinationId > 27 || destinationId < 0)
		{
			std::cout << "[Error] Invalid location：" << subCmd << "\n";
			return;
		}

		const auto &config = GameConfig::getInstance();
		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player &player = game.getPlayers()[currentPlayerIndex];

		if (player.isInHospital())
		{
			player.leaveHospital();
			std::cout << "[Cheat] " << player.getName() << " was released from the hospital!\n";
		}

		int currentPositionId = player.getPositionId();
		int totalSteps = (destinationId - currentPositionId + 28) % 28; // Wrap around the board

		if (config.getAnimation())
		{
			// Move the player step-by-step
			for (int i = 0; i < totalSteps; ++i)
			{
				player.move(1); // Move one step at a time
				Utils::clearScreen();
				game.getMap().drawBoard(game.getPlayers());
				std::this_thread::sleep_for(std::chrono::milliseconds(config.getAnimationTime())); // Optional: Add delay for animation
			}
		}
		else
		{
			// If animation is disabled, move the player directly
			player.move(totalSteps);
			game.getMap().drawBoard(game.getPlayers());
		}
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
		std::cout << "[Cheat] " << player.getName() << " moved to position " << destinationId << "\n";

		// Check if passing start including if the player is moving to the same position
		if (totalSteps == 0 || (totalSteps > 0 && ((currentPositionId + totalSteps) >= 28 || (currentPositionId + totalSteps) % 28 < currentPositionId)))
		{
			player.addMoney(config.getPassingStartBonus());
			std::cout << "[系統] " << player.getName() << " 通過起點，獲得 $" << config.getPassingStartBonus() << "!\n";
			game.getMap().drawBoard(game.getPlayers());
		}

		// Handle events after reaching the destination
		game.handleTileEvents(player);

		// Switch to next player and set state to START
		game.setGameState(GameState::START);
		int nextPlayer = (game.getCurrentPlayerIndex() + 1) % game.getPlayers().size();
		game.setCurrentPlayerIndex(nextPlayer);
		return;
	}
	else if (cmd == "/get")
	{
		std::string targetName;
		int amount;

		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player &currentPlayer = game.getPlayers()[currentPlayerIndex];

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

			Player &targetPlayer = game.getPlayers()[targetPlayerIndex];
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
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (cmd == "/give")
	{
		std::string targetName;
		int amount;
		iss >> targetName >> amount;

		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player &currentPlayer = game.getPlayers()[currentPlayerIndex];

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

		Player &targetPlayer = game.getPlayers()[targetPlayerIndex];

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

		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (cmd == "/card")
	{
		std::string cardName;
		std::getline(iss, cardName);

		// Trim leading spaces safely
		size_t firstChar = cardName.find_first_not_of(" ");

		if (firstChar == std::string::npos)
		{
			std::cout << "[Error] Please input a valid card name, ex. /card Destroy Card\n";
			return;
		}
		cardName = cardName.substr(firstChar);

		// Additional check if string is still empty
		if (cardName.empty())
		{
			std::cout << "[Error] Name can't be empty.\n";
			return;
		}

		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player &currentPlayer = game.getPlayers()[currentPlayerIndex];

		// 將指定名稱的卡牌加入玩家卡牌列表
		currentPlayer.addCard(Card(cardName));
		std::cout << "[作弊] " << currentPlayer.getName() << " 獲得卡牌：" << cardName << "\n";

		// 更新遊戲畫面
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
		return; // 不切換回合
	}
	else if (cmd == "/info")
	{
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());

		// 獲取遊戲單例
		const std::vector<Player> &players = game.getPlayers();			// 獲取所有玩家
		std::cout << "No. of players: " << players.size() << std::endl; // 打印玩家數量

		if (players.empty())
		{
			std::cerr << "No players available!\n";
			return;
		}
		for (const Player &player : players)
		{
			player.showInfo();		// 呼叫每個玩家的 showInfo() 函式
			std::cout << std::endl; // 分隔不同玩家資訊
		}
	}
	else if (cmd == "/refresh")
	{
		Utils::clearScreen();
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

		if (stateStr == "INIT")
		{
			newState = GameState::INIT;
		}
		else if (stateStr == "START")
		{
			newState = GameState::START;
		}
		else if (stateStr == "MOVED")
		{
			newState = GameState::MOVED;
		}
		else if (stateStr == "ROUND_END")
		{
			newState = GameState::ROUND_END;
		}
		else if (stateStr == "FINISH")
		{
			newState = GameState::FINISH;
		}
		else
		{
			std::cout << "Invalid input. Please enter: INIT, START, MOVED, ROUND_END, FINISH\n";
			return;
		}

		game.setGameState(newState);
		std::cout << "[Cheat] Changed the game state into " << stateStr << "\n";

		Utils::pressEnterToContinue();
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
	}
	else if (cmd == "/minigame")
	{
		int currentPlayerIndex = game.getCurrentPlayerIndex();
		Player &currentPlayer = game.getPlayers()[currentPlayerIndex];

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
			if (std::cin.fail())
			{
				// Clear the error flag
				std::cin.clear();
				// Discard invalid input
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}
			else
			{
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			}

			switch (choice)
			{
			case 1:
				std::cout << "\n[Minigame] Entering Horse Racing Game！\n";
				MiniGame::playHorseRace(currentPlayer);
				break;
			case 2:
				std::cout << "\n[Minigame] Entering Dragon Gate Game！\n";
				MiniGame::playDragonGate(currentPlayer);
				break;
			case 3:
				std::cout << "\n[Minigame] Entering Treasure Hunt Game！\n";
				MiniGame::playTreasureHunt(currentPlayer);
				break;
			case 4:
				std::cout << "\n[Minigame] Entering Maze Escape Game！\n";
				MiniGame::playMazeEscape(currentPlayer);
				break;
			case 5:
				std::cout << "[Return] Returning to the main board。\n";
				break;
			default:
				std::cout << "[Error] Invalid choice，please input 1-5。\n";
				break;
			}

			// 更新地圖顯示
			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
			return;
		}
	}
	else if (cmd == "/list" || cmd == "/help")
	{
		Utils::clearScreen();
		game.getMap().drawBoard(game.getPlayers());
		std::cout << "\n=== Available Commands ===\n\n";

		// Display commands dynamically from the JSON file
		if (commandData.is_null())
		{
			loadCommandData(); // Load command data if not loaded yet
		}

		// Display commands with description and usage
		for (auto &[cmdName, cmdInfo] : commandData.items())
		{
			if (cmdName != "invalid_command")
			{
				std::cout << "/" << cmdName << "\n";
				if (cmdInfo.contains("description"))
				{
					std::cout << "  Description: " << cmdInfo["description"].get<std::string>() << "\n";
				}
				if (cmdInfo.contains("usage"))
				{
					std::cout << "  Usage: " << cmdInfo["usage"].get<std::string>() << "\n";
				}
				if (cmdInfo.contains("examples") && cmdInfo["examples"].is_array())
				{
					std::cout << "  Examples: " << cmdInfo["examples"][0].get<std::string>();
					for (size_t i = 1; i < cmdInfo["examples"].size(); ++i)
					{
						std::cout << ", " << cmdInfo["examples"][i].get<std::string>();
					}
					std::cout << "\n";
				}
				std::cout << "\n";
			}
		}
	}
	else if (cmd == "/save")
	{
		std::string filename;
		std::cout << "Enter filename to save: ";
		std::cin >> filename;
		std::cin.ignore();
		if (game.saveGame(filename))
		{
			std::cout << "Game saved to " << filename << "\n";
		}
		else
		{
			std::cout << "Failed to save game.\n";
		}
		return;
	}
	else if (cmd == "/load")
	{
		std::string filename;
		std::cout << "Enter filename to load: ";
		std::cin >> filename;
		std::cin.ignore();
		if (game.loadGame(filename))
		{
			std::cout << "Game loaded from " << filename << "\n";
			Utils::pressEnterToContinue();
			Utils::clearScreen();
			game.getMap().drawBoard(game.getPlayers());
			game.checkWinCondition();
		}
		else
		{
			std::cout << "Failed to load game.\n";
		}

		return;
	}
	else
	{
		std::cout << "Unknown command.\n";
	}

	game.checkWinCondition();
}