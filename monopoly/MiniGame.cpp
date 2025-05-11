#include "Utils.h"
#include "MiniGame.h"
#include "Game/Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>
#include <limits>
#include <random>

void MiniGame::playHorseRace(Player &player)
{
	Utils::clearScreen();
	std::cout << "--- MiniGame: Horse Race ---\n";
	std::cout << "Four horses will race. Choose one to bet on (1-4): ";

	int betHorse = 0;
	while (betHorse < 1 || betHorse > 4)
	{
		std::cin >> betHorse;
		if (betHorse < 1 || betHorse > 4)
			std::cout << "Invalid horse. Pick 1 to 4: ";
	}

	const int finishLine = 50;
	int positions[4] = {0, 0, 0, 0};

	bool finished = false;
	int winner = -1;

	srand(static_cast<unsigned>(time(nullptr)));

	while (!finished)
	{
		Utils::clearScreen();
		std::cout << "--- Horse Race ---\n";

		for (int i = 0; i < 4; ++i)
		{
			int step = rand() % 5 + 1; // 1 to 5 steps
			positions[i] += step;
			if (positions[i] >= finishLine && winner == -1)
			{
				winner = i + 1;
				finished = true;
			}
			std::cout << "Horse " << (i + 1) << ": ";
			for (int j = 0; j < positions[i] && j < finishLine; ++j)
				std::cout << "=";
			std::cout << ">\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	std::cout << "\nHorse " << winner << " wins the race!\n";

	if (betHorse == winner)
	{
		std::cout << "You won the bet! You gain $1000!\n";
		player.addMoney(1000);
	}
	else
	{
		std::cout << "You lost the bet. You lose $500.\n";
		player.subtractMoney(500);
	}

	Utils::pressEnterToContinue();
	Utils::clearScreen();
}

void MiniGame::playDragonGate(Player &player)
{
	Utils::clearScreen();
	srand(static_cast<unsigned>(time(nullptr)));
	std::cout << "--- MiniGame: Dragon Gate ---\n";
	int card1 = rand() % 13 + 1;
	int card2 = rand() % 13 + 1;
	std::cout << "First card: " << card1 << ", Second card: " << card2 << "\n";

	int wager = 0;
	while (true)
	{
		std::cout << "Enter your bet (up to $1000): ";
		std::cin >> wager;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // flush bad input
			std::cout << "Invalid input. Please enter a number.\n";
			continue;
		}

		if (wager <= 0)
		{
			std::cout << "Bet must be greater than zero.\n";
		}
		else if (wager > 1000)
		{
			std::cout << "Bet must not be greater than 1000.\n";
		}
		else
		{
			std::cout << "You bet $" << wager << ".\n";
			break;
		}
	}

	int nextCard = rand() % 13 + 1;
	bool correct = false;
	bool pillarHit = (nextCard == card1 || nextCard == card2);

	if (card1 != card2)
	{
		int low, high;
		if (card1 < card2)
		{
			low = card1;
			high = card2;
		}
		else
		{
			low = card2;
			high = card1;
		}

		std::string guess;
		std::cout << "Do you think the next card will be [inside] or [outside] the range? ";
		std::cin >> guess;

		std::cout << "Next card: " << nextCard << "\n";

		if (pillarHit)
		{
			std::cout << "Pillar hit! You lose 2x your bet!\n";
			player.subtractMoney(wager * 2);
			return;
		}

		if (guess == "inside")
		{
			correct = (nextCard > low && nextCard < high);
		}
		else
		{
			correct = (nextCard < low || nextCard > high);
		}

		if (correct)
		{
			std::cout << "You guessed right! You win your bet!\n";
			player.addMoney(wager);
		}
		else
		{
			std::cout << "Wrong guess! You lose your bet.\n";
			player.subtractMoney(wager);
		}
	}
	else
	{
		std::string guess;
		std::cout << "Cards are equal. Will the next card be [higher] or [lower]? ";
		std::cin >> guess;

		std::cout << "Next card: " << nextCard << "\n";

		if (pillarHit)
		{
			std::cout << "Pillar hit! You lose 3x your bet!\n";
			player.subtractMoney(wager * 3);
			return;
		}

		if (guess == "higher")
		{
			correct = (nextCard > card1);
		}
		else
		{
			correct = (nextCard < card1);
		}

		if (correct)
		{
			std::cout << "You guessed right! You win your bet!\n";
			player.addMoney(wager);
		}
		else
		{
			std::cout << "Wrong guess! You lose your bet.\n";
			player.subtractMoney(wager);
		}
	}

	Utils::pressEnterToContinue();
	Utils::clearScreen();
}

void MiniGame::playTreasureHunt(Player &player)
{
	Utils::clearScreen();
	std::cout << "--- MiniGame: Treasure Hunt ---\n";
	std::cout << "Three chests are in front of you. One contains a treasure, and the others are empty.\n";
	std::cout << "Choose a chest (1, 2, or 3): ";

	int chosenChest = 0;
	while (chosenChest < 1 || chosenChest > 3)
	{
		std::cin >> chosenChest;
		if (chosenChest < 1 || chosenChest > 3)
			std::cout << "Invalid choice. Pick a chest (1, 2, or 3): ";
	}

	// Randomly determine which chest contains the treasure
	srand(static_cast<unsigned>(time(nullptr)));
	int treasureChest = rand() % 3 + 1;

	if (chosenChest == treasureChest)
	{
		// Random reward between $500 and $2000
		int reward = (rand() % 1501 + 500);
		std::cout << "Congratulations! You found the treasure and won $" << reward << "!\n";
		player.addMoney(reward);
	}
	else
	{
		int penalty = 500;
		std::cout << "Oops! The treasure was in chest " << treasureChest << ". You lose $" << penalty << ".\n";
		player.subtractMoney(penalty);
	}

	Utils::pressEnterToContinue();
	Utils::clearScreen();
}

// Maze Escape MiniGame using Depth-First Search (DFS) for maze generation
void MiniGame::playMazeEscape(Player &player)
{
	Utils::clearScreen();
	std::cout << "--- MiniGame: Maze Escape ---\n";
	std::cout << "Navigate through the maze to reach the exit (E)!\n";

	const int mazeSize = 10; // Larger maze size
	char maze[mazeSize][mazeSize];
	bool visited[mazeSize][mazeSize] = {false};
	int playerX = 0, playerY = 0;					// Player starts at the top-left corner
	int exitX = mazeSize - 1, exitY = mazeSize - 1; // Exit is at the bottom-right corner

	// Initialize the maze with walls ('#')
	for (int i = 0; i < mazeSize; ++i)
	{
		for (int j = 0; j < mazeSize; ++j)
		{
			maze[i][j] = '#';
		}
	}

	// Generate a solvable maze using Depth-First Search (DFS)
	std::function<void(int, int)> generateMaze = [&](int x, int y)
	{
		visited[x][y] = true;
		maze[x][y] = ' '; // Mark the current cell as a path

		// Randomize the order of directions
		std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::shuffle(directions.begin(), directions.end(), rng);

		for (const auto &[dx, dy] : directions)
		{
			int nx = x + dx * 2, ny = y + dy * 2; // Move two steps in the chosen direction
			if (nx >= 0 && nx < mazeSize && ny >= 0 && ny < mazeSize && !visited[nx][ny])
			{
				maze[x + dx][y + dy] = ' '; // Remove the wall between cells
				generateMaze(nx, ny);
			}
		}
	};

	generateMaze(0, 0); // Start generating the maze from the top-left corner

	// Ensure a direct path from start (P) to exit (E)
	int x = 0, y = 0;
	while (x != exitX || y != exitY)
	{
		maze[x][y] = ' '; // Mark the path as clear
		if (x < exitX)
			x++; // Move down if not at the same row as the exit
		else if (y < exitY)
			y++; // Move right if not at the same column as the exit
	}
	maze[exitX][exitY] = 'E'; // Mark the exit

	// Calculate the minimal path using BFS
	auto calculateMinimalPath = [&]() -> int
	{
		std::queue<std::pair<int, int>> q;
		int distance[mazeSize][mazeSize] = {{0}};
		bool visitedBFS[mazeSize][mazeSize] = {{false}};

		q.push({0, 0});
		visitedBFS[0][0] = true;

		std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
		while (!q.empty())
		{
			auto [cx, cy] = q.front();
			q.pop();

			for (const auto &[dx, dy] : directions)
			{
				int nx = cx + dx, ny = cy + dy;
				if (nx >= 0 && nx < mazeSize && ny >= 0 && ny < mazeSize && maze[nx][ny] != '#' && !visitedBFS[nx][ny])
				{
					visitedBFS[nx][ny] = true;
					distance[nx][ny] = distance[cx][cy] + 1;
					q.push({nx, ny});

					if (nx == exitX && ny == exitY)
						return distance[nx][ny]; // Return the minimal path length
				}
			}
		}
		return -1; // Should never reach here if the maze is solvable
	};

	int minimalPath = calculateMinimalPath();
	int movesLeft = minimalPath + 3; // Add 3 extra moves to the minimal path
	std::cout << "You have " << movesLeft << " moves to escape.\n";

	// Game loop
	while (movesLeft > 0)
	{
		// Display the maze
		Utils::clearScreen();
		for (int i = 0; i < mazeSize; ++i)
		{
			for (int j = 0; j < mazeSize; ++j)
			{
				if (i == playerX && j == playerY)
					std::cout << 'P'; // Player's position
				else
					std::cout << maze[i][j];
			}
			std::cout << '\n';
		}

		// Check if the player has reached the exit
		if (playerX == exitX && playerY == exitY)
		{
			std::cout << "Congratulations! You escaped the maze and won $5000!\n";
			player.addMoney(5000);
			return;
		}

		// Get the player's move
		std::cout << "Moves left: " << movesLeft << "\n";
		std::cout << "Enter your move (W = up, S = down, A = left, D = right): ";
		char move;
		std::cin >> move;

		// Update the player's position based on the move
		int newX = playerX, newY = playerY;
		if (move == 'W' || move == 'w')
			newX--;
		else if (move == 'S' || move == 's')
			newX++;
		else if (move == 'A' || move == 'a')
			newY--;
		else if (move == 'D' || move == 'd')
			newY++;
		else
		{
			std::cout << "Invalid move. Try again.\n";
			continue;
		}

		// Check if the move is valid
		if (newX >= 0 && newX < mazeSize && newY >= 0 && newY < mazeSize && maze[newX][newY] != '#')
		{
			playerX = newX;
			playerY = newY;
		}
		else
		{
			std::cout << "You hit a wall! Try a different move.\n";
		}

		movesLeft--;
	}

	// If the player runs out of moves
	std::cout << "You ran out of moves! You lose $2000.\n";
	player.subtractMoney(2000);
	std::cout << "Press Enter to return to the game...";
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	std::cin.get();
	Utils::clearScreen();
}