#pragma once

#include "ItemToolTip.h"
#include "InventoryManager.h"
#include "Potion.h"
#include "Player.h"

class Cursor
{
public:
	Cursor() {};
	Cursor(sf::RenderWindow* window, InventoryManager* manager, Input* input, Player* player);
	~Cursor() {};
	void update();
	void handleInput();
	void draw();
private:
	void updateToolTip(sf::Vector2i mousePos);
private:
	//Reference to window
	sf::RenderWindow* window;
	//Reference to inventory manager
	InventoryManager* manager;
	//Reference to input
	Input* input;
	//Reference to item tool tip
	ItemToolTip* tooltip;
	//Reference to player
	Player* player;
};

