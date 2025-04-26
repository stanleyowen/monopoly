#include "Utils.h"
#include <iostream>

void Utils::clearScreen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}
