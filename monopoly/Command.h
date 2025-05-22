#pragma once

#include <string>
#include <nlohmann/json.hpp>

class Game;

class Command
{
public:
	static void execute(Game &game, const std::string &input);
	static void loadCommandData();

private:
	static nlohmann::json commandData;
};