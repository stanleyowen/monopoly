#pragma once

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

class Utils
{
private:
	// Static member to hold the dialogue data
	static nlohmann::json dialogueData;

public:
	static void clearScreen();
	static void loadDialogueData();
	static void displayDialogue(const std::string &key);
};
