#pragma once
#include "Inventory.h"
#include "Framework/Collision.h"
#include "Chest.h";

class InventoryManager
{

public:
	InventoryManager();
	~InventoryManager();
	void addToInventories(Inventory* inventory);
	Inventory* getInventory(sf::Vector2i currentMousePos);
	void update(float dt);
	void draw(sf::RenderWindow* window);
	void addToChestInventories(Chest* chest, Inventory* inventory);
	Inventory* getChestInventory(Chest* chest);
	Inventory* getInventoryFromID(int ID);
	int getIDForInventory(Inventory* inventory);
	Chest* getChestForInventory(Inventory* inv);
private:
	std::vector<Inventory*> inventories;
	std::map<Chest*, Inventory*> chestToInventory;

};

