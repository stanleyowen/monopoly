#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>
#include <memory>

class Player;

class Card {
public:
	Card(const std::string& type);
	std::string getType() const;

	void applyEffect(Player& player, std::vector<Player>& players);

private:
	std::string type;
};

#endif // CARD_H
