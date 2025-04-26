#include "Card.h"

Card::Card(const std::string& type) : type(type) {}

std::string Card::getType() const {  // Get card type
	return type;
}

// Add other special cards and effects here