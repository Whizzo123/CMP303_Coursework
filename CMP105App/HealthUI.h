#pragma once
#include "Player.h"
#include <sstream>

class HealthUI
{

public:
	HealthUI() {};
	HealthUI(Player* player, sf::RenderWindow* window, sf::Vector2f pos);
	~HealthUI() {};
	void update();
	void draw();
private:
	Player* player;
	sf::RenderWindow* window;
	sf::Text text;
	sf::Font font;
	sf::Vector2f originPos;
};

