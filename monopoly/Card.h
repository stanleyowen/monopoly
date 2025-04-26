#pragma once
#include <string>

class Card {
public:
	Card(const std::string& type);
	std::string getType() const;

private:
	std::string type;
};
