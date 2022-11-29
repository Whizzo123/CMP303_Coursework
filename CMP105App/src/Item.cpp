#include "Item.h"

Item::Item(std::string itemName, int stackSize, ItemType myType)
{
	//Load texture from path
	texture.loadFromFile("gfx/items/" + itemName + ".png");
	setTexture(&texture);
	//Initialize gameobject variables
	setSize(sf::Vector2f(64, 64));
	//Initialize variables
	this->itemName = itemName;
	this->stackSize = stackSize;
	itemType = myType;
}

std::string Item::getItemName()
{
	return itemName;
}

int Item::getStackSize()
{
	return stackSize;
}

void Item::reduceStackSize()
{
	stackSize -= 1;
}
