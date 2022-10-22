#pragma once

#include "Framework/GameObject.h"
#include <string>
#include <iostream>


class Item : public GameObject
{
public:
	Item() { };
	Item(std::string itemName, int stackSize);
	~Item() { std::cout << "Scrapping " << itemName << std::endl; }
	std::string getItemName();
	int getStackSize();
	void reduceStackSize();
private:
	std::string itemName;
	int stackSize;
	sf::Texture texture;
};

