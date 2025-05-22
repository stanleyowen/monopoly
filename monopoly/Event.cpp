#include "Event.h"
#include <iostream>
#include <cstdlib>

void Event::trigger(Player &player)
{
	srand(static_cast<unsigned>(time(nullptr))); // Seed for random number generation
	int eventType = rand() % 3;

	switch (eventType)
	{
	case 0:
		player.addMoney(500);
		std::cout << "Lucky! You gained $500!\n";
		break;
	case 1:
		player.subtractMoney(300);
		std::cout << "Oops! You lost $300!\n";
		break;
	case 2:
		std::cout << "Nothing happened.\n";
		break;
	}
}
