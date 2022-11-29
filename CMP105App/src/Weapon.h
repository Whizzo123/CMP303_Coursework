#pragma once
#include "Item.h"
class Weapon : public Item
{
public:
	Weapon() {}
	Weapon(std::string itemName, int stackSize, int damage, int speed) : Item(itemName, stackSize, ItemType::WEAPONITEM)
	{
		this->damage = damage;
		this->speed = speed;
	}
	~Weapon() {}
	int getDamage() { return damage; }
	int getSpeed() { return speed; }

private:
	int damage;
	int speed;
};

