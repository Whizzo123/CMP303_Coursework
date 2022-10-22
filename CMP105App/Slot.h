#pragma once
#include "Framework/GameObject.h"
#include "Item.h"
#include "Weapon.h"
#include "Armour.h"
//#include "Potion.h"
#include "Framework/Collision.h"
#include <iostream>

enum SlotType {ALL, WEAPON, ARMOURHEAD, ARMOURCHEST, ARMOURLEGS, ARMOURBOOTS};

class Slot : public GameObject
{

public:
	Slot() {};
	Slot(Input* input, sf::RenderWindow* window);
	Slot(SlotType slotType, Input* input, sf::RenderWindow* window);
	~Slot() { std::cout << "Calling deconstructor " << std::endl; }
	void setItem(Item* item);
	Item* getItem() { return item; }
	SlotType getSlotType();
	void setSlotType(SlotType type);
	void setOriginPosition(sf::Vector2f position);
	void update(float dt) override;
	sf::Vector2f getOriginPosition();
	bool mouseOver();
	bool isItemCompatable(Item* item);
	void clearItem();

private:
	Item* item;
	SlotType slotType;
	sf::Texture texture;
	sf::Vector2f originPosition;
};

