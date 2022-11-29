#include "InventoryManager.h"

InventoryManager::InventoryManager()
{
	
}

InventoryManager::~InventoryManager()
{

}

void InventoryManager::addToInventories(Inventory* inventory)
{
	inventories.push_back(inventory);
}

Inventory* InventoryManager::getInventory(sf::Vector2i currentMousePos)
{
	////Grab map iterator
	std::map<Chest*, Inventory*>::iterator it = chestToInventory.begin();
	//Loop through map
	while (it != chestToInventory.end())
	{
		//If inventory is showing
		if (it->second->getShowing())
		{
			//Check for collision with mouse position
			if (Collision::checkBoundingBox(it->second, currentMousePos))
			{
				//return inventory
				return it->second;
			}
		}
		//Increment iterator
		it++;
	}
	//Loop through inventories
	for (int i = 0; i < inventories.size(); i++)
	{
		//Check for collision with mouse
		if (Collision::checkBoundingBox(inventories[i], currentMousePos))
		{
			//If so return inventory
			return inventories[i];
		}
	}
	return nullptr;
}

void InventoryManager::update(float dt)
{
	//Loop through inventories and update
	for (int i = 0; i < inventories.size(); i++)
	{
		inventories[i]->update(dt);
	}
	std::map<Chest*, Inventory*>::iterator it = chestToInventory.begin();
	//Loop through map and update inventories
	while (it != chestToInventory.end())
	{
		if (it->second->getShowing())
		{
			it->second->update(dt);
		}
		it++;
	}
}

void InventoryManager::draw(sf::RenderWindow* window)
{
	for (int i = 0; i < inventories.size(); i++)
	{
		if (inventories[i]->getShowing())
		{
			window->draw(*inventories[i]);
			inventories[i]->draw();
		}
	}
	std::map<Chest*, Inventory*>::iterator it = chestToInventory.begin();
	while (it != chestToInventory.end())
	{
		if (it->second->getShowing())
		{
			window->draw(*it->second);
			it->second->draw();
		}
		it++;
	}
}

void InventoryManager::addToChestInventories(Chest* chest, Inventory* inventory)
{
	chestToInventory[chest] = inventory;
	inventories.push_back(inventory);
}

Inventory* InventoryManager::getChestInventory(Chest* chest)
{
	return chestToInventory[chest];
}

Chest* InventoryManager::getChestForInventory(Inventory* inventory)
{
	for (auto chest : chestToInventory)
	{
		if (chest.second == inventory)
			return chest.first;
	}
	return nullptr;
}

Inventory* InventoryManager::getInventoryFromID(int id)
{
	return inventories[id];
}

int InventoryManager::getIDForInventory(Inventory* inventory)
{
	for (int i = 0; i < inventories.size(); i++)
	{
		if (inventories[i] == inventory)
			return i;
	}
}