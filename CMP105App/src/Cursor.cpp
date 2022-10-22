#include "Cursor.h"

Cursor::Cursor(sf::RenderWindow* window, InventoryManager* manager, Input* input, Player* player)
{
	//Initialize variables
	this->window = window;
	this->manager = manager;
	this->input = input;
	this->player = player;
	tooltip = new ItemToolTip("", 0);
}

void Cursor::handleInput()
{
	//Compensate mouse position due to view movement
	sf::Vector2i mousePos = sf::Vector2i(input->getMouseX() + window->getView().getCenter().x - window->getSize().x / 2,
		input->getMouseY() + window->getView().getCenter().y - window->getSize().y / 2);
	//If pressing right mouse button
	if (input->isRightMousePressed())
	{
		Inventory* inventory = manager->getInventory(mousePos);
		//If inventory below mouse and is showing
		if (inventory && inventory->getShowing())
		{
			Slot* slot = inventory->getSlotMousedOver();
			//If mousing over slot
			if (slot != nullptr)
			{
				Item* item = slot->getItem();
				//If slot contains item
				if (item != nullptr)
				{
					//If is of type potion
					if (dynamic_cast<Potion*>(item))
					{
						//Cast to potion
						Potion* potion = (dynamic_cast<Potion*>(item));
						potion->useEffect();
					}
				}
			}
		}
	}
}

void Cursor::update()
{
	//Grab mouse position with respect to view movement
	sf::Vector2i mousePos = sf::Vector2i(input->getMouseX() + window->getView().getCenter().x - window->getSize().x / 2,
		input->getMouseY() + window->getView().getCenter().y - window->getSize().y / 2);
	//Update tool tip
	Cursor::updateToolTip(mousePos);
}

void Cursor::updateToolTip(sf::Vector2i mousePos)
{
	//Set position to mouse position
	tooltip->setPosition(sf::Vector2f(mousePos.x, mousePos.y));
	//Grab inventory below mouse position
	Inventory* inventory = manager->getInventory(mousePos);
	//If has inventory and is showing
	if (inventory && inventory->getShowing())
	{
		Slot* slot = inventory->getSlotMousedOver();
		//If slot is moused over
		if (slot != nullptr)
		{
			Item* item = slot->getItem();
			//If slot has item
			if (item != nullptr)
			{
				//If of type weapon
				if (dynamic_cast<Weapon*>(item))
				{
					//Clear tooltip
					tooltip->clearChildren();
					//Cast item to weapon
					Weapon* weapon = (dynamic_cast<Weapon*>(item));
					//Populate tooltip with weapon stats
					tooltip->resetItem(weapon->getItemName(), weapon->getStackSize());
					tooltip->addToChildObjects("ATK: ", std::to_string(weapon->getDamage()));
					tooltip->addToChildObjects("SPD: ", std::to_string(weapon->getSpeed()));
					//Show tooltip
					if (!tooltip->isShowing())
						tooltip->toggle();
				}
				//If of type potion
				if (dynamic_cast<Potion*>(item))
				{
					//Clear tooltip
					tooltip->clearChildren();
					//Cast item to potion
					Potion* potion = (dynamic_cast<Potion*>(item));
					//Populate tooltip with potion stats
					potion->setHoldingCharacter(player);
					tooltip->resetItem(potion->getItemName(), potion->getStackSize());
					tooltip->addToChildObjects("HP: ", std::to_string(3));
					//Show tooltip
					if (!tooltip->isShowing())
						tooltip->toggle();
				}
				//If of type armour
				if (dynamic_cast<Armour*>(item))
				{
					//Clear tooltip
					tooltip->clearChildren();
					//Cast item to armour
					Armour* armour = (dynamic_cast<Armour*>(item));
					//Populate tooltip with armour stats
					tooltip->resetItem(armour->getItemName(), armour->getStackSize());
					tooltip->addToChildObjects("ARM: ", std::to_string(armour->getArmourValue()));
					//Show tooltip
					if (!tooltip->isShowing())
						tooltip->toggle();
				}
			}
			else
			{
				//Disable tooltip
				if (tooltip->isShowing())
					tooltip->toggle();
			}
		}
		else
		{
			//Disable tooltip
			if (tooltip->isShowing())
				tooltip->toggle();
		}
	}
	else
	{
		//Disable tooltip
		if (tooltip->isShowing())
			tooltip->toggle();
	}
}

void Cursor::draw()
{
	if (tooltip->isShowing())
	{
		window->draw(*tooltip);
		tooltip->draw(window);
	}
}