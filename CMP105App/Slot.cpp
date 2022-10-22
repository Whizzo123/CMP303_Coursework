#include "Slot.h"


Slot::Slot(Input* input, sf::RenderWindow* window)
{
	//Load texture from path
	texture.loadFromFile("gfx/slot.png");
	setTexture(&texture);
	//Initialize gameobject variables
	setSize(sf::Vector2f(64, 64));
	setCollisionBox(sf::FloatRect(0, 0, getSize().x, getSize().y));
	setInput(input);
	setWindow(window);
	//Initialize variables
	setSlotType(ALL);
	setItem(nullptr);
}

Slot::Slot(SlotType slotType, Input* input, sf::RenderWindow* window)
{
	//Load texture from path
	texture.loadFromFile("gfx/slot.png");
	setTexture(&texture);
	//Initialize gameobject variables
	setSize(sf::Vector2f(64, 64));
	setCollisionBox(sf::FloatRect(0, 0, getSize().x, getSize().y));
	setInput(input);
	setWindow(window);
	//Initialize variables
	this->slotType = slotType;
	setItem(nullptr);
}

SlotType Slot::getSlotType()
{
	return slotType;
}

void Slot::setSlotType(SlotType slotType)
{
	this->slotType = slotType;
}

sf::Vector2f Slot::getOriginPosition()
{
	return originPosition;
}

void Slot::setOriginPosition(sf::Vector2f position)
{
	originPosition = position;
}

void Slot::update(float dt)
{
	//If has item
	if (item != nullptr)
	{
		//If stack size if greater than zero update position
		if (item->getStackSize() > 0)
			item->setPosition(getPosition());
		//Can't have 0 items in stack remove from inventory
		else
			clearItem();
	}
}

bool Slot::mouseOver()
{
	//Grab mouse position with respect to view
	sf::Vector2i mousePos = sf::Vector2i(input->getMouseX() + window->getView().getCenter().x - window->getSize().x / 2,
		input->getMouseY() + window->getView().getCenter().y - window->getSize().y / 2);
	//Check for collision with mouse position
	if (Collision::checkBoundingBox(this, mousePos))
	{
		//Highlight in red
		setFillColor(sf::Color::Red);
		return true;
	}
	else
	{
		//Return to normal colour
		setFillColor(sf::Color::White);
		return false;
	}
}

void Slot::clearItem()
{
	//If slot contains item
	if (item != nullptr)
		//Clear it
		item = nullptr;
}

bool Slot::isItemCompatable(Item* item)
{
	switch (slotType)
	{
	//If slot type is all just return true
	case ALL:
		return true;
		break;
	//If slot type is weapon and item type is weapon return true
	case WEAPON:
		if (dynamic_cast<Weapon*>(item))
			return true;
		else
			return false;
		break;
	//If slot type is armourhead and item type is armourhead return true
	case ARMOURHEAD:
		if (dynamic_cast<Armour*>(item))
		{
			if (dynamic_cast<Armour*>(item)->getArmourType() == HEAD)
				return true;
			else
				return false;
		}
		else
			return false;
		break;
	//If slot type is armourchest and item type is armourchest return true
	case ARMOURCHEST:
		if (dynamic_cast<Armour*>(item))
		{
			if (dynamic_cast<Armour*>(item)->getArmourType() == CHEST)
				return true;
			else
				return false;
		}
		else
			return false;
		break;
	//If slot type is arourlegs and item type is armourlegs return true
	case ARMOURLEGS:
		if (dynamic_cast<Armour*>(item))
		{
			if (dynamic_cast<Armour*>(item)->getArmourType() == LEGS)
				return true;
			else
				return false;
		}
		else
			return false;
		break;
	//If slot type is armourboots and item type is armourboots return true
	case ARMOURBOOTS:
		if (dynamic_cast<Armour*>(item))
		{
			if (dynamic_cast<Armour*>(item)->getArmourType() == FEET)
				return true;
			else
				return false;
		}
		else
			return false;
		break;
	}
}

void Slot::setItem(Item* item)
{
	this->item = item;
	if (item != nullptr)
	{
		this->item->setPosition(getPosition());
	}
}