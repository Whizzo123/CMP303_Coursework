#pragma once

#include "Framework/GameObject.h"
#include <string>
#include <iostream>

enum ItemType {WEAPONITEM, ARMOURITEM, POTIONITEM};

class Item : public GameObject
{
public:
	Item() { };
	Item(std::string itemName, int stackSize, ItemType myType);
	~Item() { std::cout << "Scrapping " << itemName << std::endl; }
	std::string getItemName();
	int getStackSize();
	void reduceStackSize();
	ItemType getItemType() { return itemType; }
private:
	std::string itemName;
	int stackSize;
	sf::Texture texture;
	ItemType itemType;
};

