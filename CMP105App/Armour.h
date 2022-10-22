#pragma once
#include "Item.h"

enum ArmourType {HEAD, CHEST, LEGS, FEET };

class Armour : public Item
{
public:
	Armour();
	Armour(std::string itemName, int stackSize, int armourValue, ArmourType armourType);
	~Armour();
	int getArmourValue();
	ArmourType getArmourType();

private:
	//Higher the armourValue more protection player will get
	int armourValue;
	ArmourType armourType;
};

