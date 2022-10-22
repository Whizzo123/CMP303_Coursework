#pragma once

#include "SFML\Graphics.hpp""
#include "Framework/Input.h"
#include "InventoryManager.h"
#include "Potion.h"

class DragController
{
public:
	DragController();
	DragController(Input* input, InventoryManager* inventoryManager, sf::RenderWindow* window);
	void update(float dt);

private:
	//Reference to input
	Input* input;
	//Reference to inventory manager
	InventoryManager* inventoryManager;
	//Reference to item
	Item* holdingItem;
	bool dragging;
	//Reference to slot
	Slot* beginDragSlot;
	//Reference to window
	sf::RenderWindow* window;
};

