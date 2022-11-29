#pragma once

#include "Chest.h"
#include "ExtendedCollision.h"
#include "Framework/Input.h"
#include "DungeonDiverTileMap.h"
#include "InventoryManager.h"
#include "ConfigLoader.h"
#include "Potion.h"
#include "Framework/AudioManager.h"
#include "Network/NetworkingManager.h"

class ChestManager
{
public:
	ChestManager();
	ChestManager(Input* input, InventoryManager* inventoryManager, AudioManager* audio);
	~ChestManager();
	void handleInput();
	void checkToOpen(sf::FloatRect interacterRect);
	void spawn(DungeonDiverTileMap* map, sf::RenderWindow* window, Input* input, int numberToSpawn);
	void draw(sf::RenderWindow* window);
	void openChest(Chest* chest);
	void closeChest(Chest* chest);
	bool isChestOpen(Chest* chest);
	Chest* getChestFromID(int id) { return chests[id]; }
	int getIDForChest(Chest* chest);
	std::vector<Chest*> getChests() { return chests; }
	void spawnNetworkedChests(ChestSpawnInfoResult result, sf::RenderWindow* window, Input* input);
private:
	//Vector of chest pointers
	std::vector<Chest*> chests;
	//Reference to input
	Input* input;
	//Reference to inventory manager
	InventoryManager* inventoryManager;
	//Reference to audio manager
	AudioManager* audio;
};

