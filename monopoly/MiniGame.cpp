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
#include <queue>

void MiniGame::playHorseRace(Player& player)
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
	int positions[4] = { 0, 0, 0, 0 };
	int rankings[4] = { -1, -1, -1, -1 };
	int rankCounter = 1;
	bool finished = false;

	srand(static_cast<unsigned>(time(nullptr)));

	while (!finished)
	{
		Utils::clearScreen();
		std::cout << "--- Horse Race ---\n";
		std::cout << "Finish Line: ";

		for (int i = 0; i < finishLine; ++i) {
			std::cout << "-";
		}

		std::cout << "|\n";

		for (int i = 0; i < 4; ++i)
		{
			if (rankings[i] == -1)
			{
				int step = rand() % 5 + 1;
				positions[i] += step;
				if (positions[i] >= finishLine)
				{
					rankings[i] = rankCounter++;
					if (rankCounter > 4)
					{
						finished = true;
					}
				}
			}

			std::cout << "Horse " << (i + 1) << ": ";

			int barLength = (positions[i] < finishLine) ? positions[i] : finishLine;

			for (int j = 0; j < barLength; ++j) {
				std::cout << "=";
			}

			std::cout << ">";

			if (rankings[i] != -1 && positions[i] >= finishLine) {
				std::cout << "  <Rank " << rankings[i] << ">";
			}

			std::cout << "\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	int winner = std::find(rankings, rankings + 4, 1) - rankings;
	std::cout << "\nHorse " << (winner + 1) << " wins the race!\n";

	if (betHorse == winner + 1)
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

void MiniGame::playDragonGate(Player& player)
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
		std::cout << "Enter your bet (you currently have " << player.getMoney() << "): ";
		std::cin >> wager;
		std::cin.ignore();

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			std::cout << "Invalid input. Please enter a number.\n";
			continue;
		}

		if (wager <= 0)
		{
			std::cout << "Bet must be greater than zero.\n";
		}
		else if (wager > player.getMoney())
		{
			std::cout << "You do not have enough money to bet that amount!\n";
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

		while (true) {
			std::cout << "Do you think the next card will be [inside] or [outside] the range? ";
			std::cin >> guess;
			std::cin.ignore();

			if (guess == "inside" || guess == "outside") {
				break;
			}

			std::cout << "Invalid choice. Please type 'inside' or 'outside'.\n";
		}

		std::cout << "Next card: " << nextCard << "\n";

		if (pillarHit)
		{
			std::cout << "Pillar hit! You lose 2x your bet!\n";
			player.subtractMoney(wager * 2);
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

		while (true) {
			std::cout << "Cards are equal. Will the next card be [higher] or [lower]? ";
			std::cin >> guess;
			std::cin.ignore();

			if (guess == "higher" || guess == "lower") {
				break;
			}

			std::cout << "Invalid choice. Please type 'higher' or 'lower'.\n";
		}


		std::cout << "Next card: " << nextCard << "\n";

		if (pillarHit)
		{
			std::cout << "Pillar hit! You lose 3x your bet!\n";
			player.subtractMoney(wager * 3);
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

void MiniGame::playTreasureHunt(Player& player)
{
	Utils::clearScreen();
	std::cout << "--- MiniGame: Treasure Hunt ---\n";
	std::cout << "Three chests are in front of you. One contains a treasure, and the others are empty.\n";
	std::cout << "Choose a chest (1, 2, or 3): ";

	int chosenChest = 0;
	while (chosenChest < 1 || chosenChest > 3)
	{
		std::cin >> chosenChest;
		std::cin.ignore();

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

// Function to calculate the shortest path using BFS
int calculateShortestPath(char maze[10][10], int startX, int startY, int endX, int endY)
{
	std::queue<std::tuple<int, int, int>> q; // Queue to store {x, y, distance}
	bool visited[10][10] = { false };

	q.push({ startX, startY, 0 });
	visited[startX][startY] = true;

	std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

	while (!q.empty())
	{
		auto [x, y, dist] = q.front();
		q.pop();

		// If we reach the exit, return the distance
		if (x == endX && y == endY)
			return dist;

		// Explore all possible directions
		for (const auto& [dx, dy] : directions)
		{
			int nx = x + dx, ny = y + dy;
			if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10 && !visited[nx][ny] && maze[nx][ny] != '#')
			{
				q.push({ nx, ny, dist + 1 });
				visited[nx][ny] = true;
			}
		}
	}

	// If no path is found (shouldn't happen in a valid maze), return a large number
	return 100;
}

void MiniGame::playMazeEscape(Player& player)
{
	Utils::clearScreen();
	std::cout << "--- MiniGame: Maze Escape ---\n";
	std::cout << "Navigate through the maze to reach the exit (E)!\n";

	const int mazeSize = 10; // Maze size
	char maze[mazeSize][mazeSize];
	bool visited[mazeSize][mazeSize] = { false };
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
		std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::shuffle(directions.begin(), directions.end(), rng);

		for (const auto& [dx, dy] : directions)
		{
			int nx = x + dx * 2, ny = y + dy * 2; // Move two steps in the chosen direction
			if (nx >= 0 && nx < mazeSize && ny >= 0 && ny < mazeSize && !visited[nx][ny])
			{
				maze[x + dx][y + dy] = ' '; // Remove the wall between cells
				generateMaze(nx, ny);
			}
		}
	};

	generateMaze(0, 0);		  // Start generating the maze from the top-left corner
	maze[exitX][exitY] = 'E'; // Mark the exit

	// Ensure at least one cell adjacent to the exit is clear
	std::vector<std::pair<int, int>> adjacentCells = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::shuffle(adjacentCells.begin(), adjacentCells.end(), rng);

	for (const auto& [dx, dy] : adjacentCells)
	{
		int adjX = exitX + dx, adjY = exitY + dy;
		if (adjX >= 0 && adjX < mazeSize && adjY >= 0 && adjY < mazeSize)
		{
			maze[adjX][adjY] = ' '; // Clear one random adjacent cell
			break;
		}
	}

	// Calculate the shortest path from the player's starting position to the exit
	int shortestPath = calculateShortestPath(maze, playerX, playerY, exitX, exitY);

	// Set the number of moves allowed to the shortest path + 1
	int movesLeft = shortestPath + 1;

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
		std::cin.ignore();

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
	std::cin.get();
	Utils::clearScreen();
}