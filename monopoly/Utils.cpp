#include "Utils.h"
#include <fstream>

nlohmann::json Utils::dialogueData;

void Utils::loadDialogueData()
{
	std::string dialoguePath = "json/dialogue.json";
	std::ifstream fileDialogue(dialoguePath);

	if (!fileDialogue.is_open())
	{
		std::cerr << "Cannot open dialogue file: " << dialoguePath << "\n";
		return;
	}

	fileDialogue >> dialogueData;
}

void Utils::clearScreen()
{
#ifdef _WIN32
	std::system("cls");
#else
	std::system("clear");
#endif
}

void Utils::displayDialogue(const std::string &key)
{
	if (!dialogueData.contains(key))
	{
		std::cerr << "Dialogue key not found: " << key << "\n";
		return;
	}

	const auto &dialogue = dialogueData[key];
	std::cout << dialogue["prompt"].get<std::string>() << "\n";

	if (dialogue.contains("options"))
	{
		for (const auto &option : dialogue["options"])
		{
			std::cout << "[" << option["key"].get<std::string>() << "] " << option["description"].get<std::string>() << "\n";
		}
	}
}