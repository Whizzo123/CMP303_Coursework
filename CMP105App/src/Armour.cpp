#include "Armour.h"

Armour::Armour() {}
Armour::~Armour() {}

Armour::Armour(std::string itemName, int stackSize, int armourValue, ArmourType armourType) : Item(itemName, stackSize)
{
	this->armourValue = armourValue;
	this->armourType = armourType;
}

//Returns type of armour (HEAD, CHEST, LEGS or FEET)
ArmourType Armour::getArmourType()
{
	return armourType;
}
//Returns armourValue for armour piece
int Armour::getArmourValue()
{
	return armourValue;
}