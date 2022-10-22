#pragma once
#include "Item.h"
#include "Character.h"
class Potion : public Item
{
public:
	Potion() {};
	Potion(std::string potionName, int stackSize, Character* character);
	~Potion() {};
	void useEffect();
	void setHoldingCharacter(Character* character);
private:
	Character* character;
};

