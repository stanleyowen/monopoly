#pragma once
#include <string>

class Game;

class Command {
public:
	static void execute(Game& game, const std::string& input);
};
