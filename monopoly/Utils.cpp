#include "Utils.h"
#include <fstream>
#include <sstream>

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

void Utils::displayDialogue(const std::string& key)
{
	// Split the key into parts for nested access
	std::istringstream keyStream(key);
	std::string part;
	nlohmann::json current = dialogueData;

	while (std::getline(keyStream, part, '.'))
	{
		if (!current.contains(part))
		{
			std::cerr << "Dialogue key not found: " << key << "\n";
			std::cerr << "Available keys: ";
			for (auto it = current.begin(); it != current.end(); ++it)
			{
				std::cerr << it.key() << " ";
			}
			std::cerr << "\n";
			return;
		}
		current = current[part];
	}

	// Display the prompt
	if (current.contains("prompt"))
	{
		std::cout << current["prompt"].get<std::string>() << "\n";
	}

	// Display the options
	if (current.contains("options"))
	{
		for (const auto& option : current["options"])
		{
			std::cout << "[" << option["key"].get<std::string>() << "] " << option["description"].get<std::string>() << "\n";
		}
	}
}

void Utils::pressEnterToContinue()
{
	std::cout << "Press Enter to continue...\n";
	std::cin.get();
}