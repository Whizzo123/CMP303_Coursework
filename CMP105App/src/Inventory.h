#pragma once
#include "Framework/GameObject.h"
#include "Slot.h"
#include <iostream>

class Inventory : public GameObject
{

public:
	Inventory(sf::RenderWindow* window, Input* input);
	Inventory(sf::RenderWindow* window, Input* input, int inventorySize);
	~Inventory() {}
	void update(float dt);
	void draw();
	Slot* getSlotMousedOver();
	void addToSlot(int slotIndex, Item* item);
	void addToRandomSlot(Item* item);
	Slot* getSlot(int slotIndex);
	void setShowing(bool value);
	bool getShowing();
	int grabArmourValue();
	int getSlotID(Slot* slot);
	int getInventorySize();
	int getNumOfOccupiedSlots();
	
private:
	Slot* slots;
	sf::Texture texture;
	int inventorySize;
	sf::Vector2f originPosition;
	bool showing;

};

