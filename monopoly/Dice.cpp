#include "Dice.h"
#include <cstdlib>
#include <ctime>

int Dice::roll() {
	static bool initialized = false;
	if (!initialized) {
		srand(time(0));
		initialized = true;
	}
	return (rand() % 6 + 1) + (rand() % 6 + 1);
}
