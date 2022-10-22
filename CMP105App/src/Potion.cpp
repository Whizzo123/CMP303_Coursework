#include "Potion.h"

Potion::Potion(std::string potionName, int stackSize, Character* character) : Item(potionName, stackSize) 
{
	this->character = character;
}

void Potion::setHoldingCharacter(Character* character)
{
	this->character = character;
}

void Potion::useEffect()
{
	//Heal character and reduce number of potions in stack
	character->heal(3);
	reduceStackSize();
}