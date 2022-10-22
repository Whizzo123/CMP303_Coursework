#include "Inventory.h"


Inventory::Inventory(sf::RenderWindow* window, Input* input)
{
	//Load in texture of inventory
	texture.loadFromFile("gfx/inventory.png");
	setTexture(&texture);
	//Initialize gameobject variables
	setSize(sf::Vector2f(420, 312));
	originPosition = sf::Vector2f(0, 0);
	setPosition(originPosition);
	setCollisionBox(sf::FloatRect(0, 0, getSize().x, getSize().y));
	setWindow(window);
	//Initialize variables
	inventorySize = 11;
	showing = false;
	slots = new Slot[inventorySize];
	//Setup slots for player inventory
	for (int i = 0; i < inventorySize; i++)
	{
		slots[i] = *(new Slot(ALL, input, window));
	}
	float offset = 7.0f;
	//Set up armour slots
	for (int i = 0; i < 4; i++)
	{
		slots[i].setSlotType(SlotType(ARMOURHEAD + i));
		slots[i].setOriginPosition(sf::Vector2f(getPosition().x + 10, getPosition().y + 25 + (i * offset) + (i * slots[i].getSize().y)));
	}
	int j = 4;
	//Set up normal slots
	while (j < 10)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int x = 0; x < 3; x++)
			{
				slots[j].setSlotType(ALL);
				slots[j].setOriginPosition(sf::Vector2f(getPosition().x + 100.0f + (x * offset) + (x * slots[j].getSize().x),
					getPosition().y + 75 + (y * (slots[j].getSize().y + 10.0f))));
				j++;
			}
		}
	}
	//Set up weapon slot
	slots[10].setSlotType(WEAPON);
	slots[10].setOriginPosition(sf::Vector2f(getPosition().x + getSize().x - slots[10].getSize().x - 15.0f, getPosition().y + getSize().y - slots[10].getSize().y - 15.0f));

}

Inventory::Inventory(sf::RenderWindow* window, Input* input, int inventorySize)
{
	//Load in texture from path
	texture.loadFromFile("gfx/inventory.png");
	//Initialize gameobject variables
	setTexture(&texture);
	setSize(sf::Vector2f(320, 180));
	originPosition = sf::Vector2f(window->getSize().x - getSize().x, 0);
	setPosition(originPosition);
	setCollisionBox(sf::FloatRect(0, 0, getSize().x, getSize().y));
	setWindow(window);
	//Initialize variables
	slots = new Slot[inventorySize];
	this->inventorySize = inventorySize;
	showing = false;
	//Setup normal slots for chest inventory
	for (int i = 0; i < inventorySize; i++)
	{
		slots[i] = *(new Slot(ALL, input, window));
		slots[i].setOriginPosition(sf::Vector2f(getPosition().x + (i * slots[i].getSize().x) + (i * 30.0f) + 30.0f, getPosition().y + (getSize().y / 2) - (slots[i].getSize().y / 2)));
	}
}

void Inventory::draw()
{
	for (int i = 0; i < inventorySize; i++)
	{
		window->draw(slots[i]);
		if(slots[i].getItem() != nullptr)
			window->draw(*slots[i].getItem());
	}
}

void Inventory::update(float dt)
{
	//Update inventory and slot positions for view 
	setPosition(originPosition + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2, window->getView().getCenter().y - window->getSize().y / 2));
	for (int i = 0; i < inventorySize; i++)
	{
		slots[i].setPosition(slots[i].getOriginPosition().x + window->getView().getCenter().x - window->getSize().x / 2, 
			slots[i].getOriginPosition().y + window->getView().getCenter().y - window->getSize().y / 2);
		slots[i].update(dt);
	}
}

Slot* Inventory::getSlotMousedOver()
{
	//Loop through slots and check for slot moused over
	for (int i = 0; i < inventorySize; i++)
	{
		if (slots[i].mouseOver())
		{
			//return moused over slot
			return &slots[i]; 
		}
	}
	return nullptr;
}

void Inventory::addToSlot(int slotIndex, Item* item)
{
	//Check if slot can accept item
	if (slots[slotIndex].isItemCompatable(item))
	{
		slots[slotIndex].setItem(item);
	}
}

void Inventory::addToRandomSlot(Item* item)
{
	//Loop through all slots in inventory
	for (int i = 0; i < inventorySize; i++)
	{
		//It slot does not have item
		if (slots[i].getItem() == nullptr)
		{
			//Set slot item
			slots[i].setItem(item);
			return;
		}
	}
}

Slot* Inventory::getSlot(int slotIndex)
{
	return &slots[slotIndex];
}

bool Inventory::getShowing()
{
	return showing;
}

void Inventory::setShowing(bool value)
{
	showing = value;
}

int Inventory::grabArmourValue()
{
	int totalArmourValue = 0;
	//Loop through inventory slots
	for (int i = 0; i < inventorySize; i++)
	{
		//If slot is of an armour type
		if (slots[i].getSlotType() == ARMOURHEAD || slots[i].getSlotType() == ARMOURCHEST || slots[i].getSlotType() == ARMOURLEGS 
			|| slots[i].getSlotType() == ARMOURBOOTS)
		{
			//Add to total armour value of armour in slot
			totalArmourValue += dynamic_cast<Armour*>(slots[i].getItem())->getArmourValue();
		}
	}
	return totalArmourValue;
}