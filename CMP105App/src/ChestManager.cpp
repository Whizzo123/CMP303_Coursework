#include "ChestManager.h"
ChestManager::ChestManager() {}

ChestManager::ChestManager(Input* input, InventoryManager* inventoryManager, AudioManager* audio)
{
	//Initialize variables
	this->input = input;
	this->inventoryManager = inventoryManager;
	this->audio = audio;
	//Generate weapons and armour from files
	ConfigLoader::loadWeaponsFromFile();
	ConfigLoader::loadArmoursFromFile();
}

ChestManager::~ChestManager()
{

}

void ChestManager::spawn(DungeonDiverTileMap* map, sf::RenderWindow* window, Input* input, int numberToSpawn)
{
	//For number of chests to spawn
	for (int i = 0; i < numberToSpawn; i++)
	{
		//Find chest spawn position
		sf::Vector2f spawnPos = map->findRandomSpawnPos();
		//Add to chest vector
		chests.push_back(new Chest("Chest_Closed", spawnPos, sf::Vector2f(75, 75), window, input));
		//Add to inventory map
		inventoryManager->addToChestInventories(chests[i], new Inventory(window, input, 3));
		//Loop 2 times
		for (int j = 0; j < 2; j++)
		{
			//Pick random number between 0 and 2
			int random = rand() % 3;
			switch (random)
			{
			case 0:
			{
				//Get random weapon
				int index = rand() % (ConfigLoader::getWeaponsLoaded().size() - 1);
				GameWeapons weaponProto = ConfigLoader::getWeaponsLoaded()[index];
				//Add weapon to slot in chest
				inventoryManager->getChestInventory(chests[i])->addToRandomSlot(new Weapon(weaponProto.name, 1, weaponProto.damage, weaponProto.speed));
				break;
			}
			case 1:
			{
				//Get random armour piece
				int index = rand() % (ConfigLoader::getArmoursLoaded().size() - 1);
				GameArmour armourProto = ConfigLoader::getArmoursLoaded()[index];
				//Add armour piece to slot in chest
				inventoryManager->getChestInventory(chests[i])->addToRandomSlot(new Armour(armourProto.name, 1, armourProto.armourValue, ArmourType(armourProto.armourType)));
				break;
			}
			case 2:
				//Add health potion to chest
				inventoryManager->getChestInventory(chests[i])->addToRandomSlot(new Potion("Health Potion", 1, nullptr));
				break;
			}
		}
	}
	std::cout << "Spawning server chests: " << numberToSpawn << std::endl;
}

void ChestManager::spawnNetworkedChests(ChestSpawnInfoResult result, sf::RenderWindow* window, Input* input)
{
	for (int i = 0; i < result.length; i++)
	{
		ChestSpawnInfo info = result.chestsInfo[i];
		//Grab chest spawn position
		sf::Vector2f spawnPos = info.position;
		//Add to chest vector
		chests.push_back(new Chest("Chest_Closed", spawnPos, sf::Vector2f(75, 75), window, input));
		//Add to inventory map
		inventoryManager->addToChestInventories(chests[i], new Inventory(window, input, 3));
		for (int j = 0; j < info.itemsCount; j++)
		{
			ItemData data = info.items[j];
			int index = data.itemIndex;
			switch (data.itemType)
			{
			case 0:
			{
				GameWeapons weaponProto = ConfigLoader::getWeaponsLoaded()[index];
				//Add weapon to slot in chest
				inventoryManager->getChestInventory(chests[i])->addToRandomSlot(new Weapon(weaponProto.name, 1, weaponProto.damage, weaponProto.speed));
				break;
			}
			case 1:
			{
				GameArmour armourProto = ConfigLoader::getArmoursLoaded()[index];
				//Add armour piece to slot in chest
				inventoryManager->getChestInventory(chests[i])->addToRandomSlot(new Armour(armourProto.name, 1, armourProto.armourValue, ArmourType(armourProto.armourType)));
				break;
			}
			case 2:
				//Add health potion to chest
				inventoryManager->getChestInventory(chests[i])->addToRandomSlot(new Potion("Health Potion", 1, nullptr));
				break;
			}
		}
	}
	std::cout << "Spawning networked chests" << chests.size() << std::endl;
}

void ChestManager::handleInput()
{
	//If pressing e key
	if (input->isPressed(sf::Keyboard::E))
	{
		//For all chests
		for (int i = 0; i < chests.size(); i++)
		{
			//If chest is open close it
			if (isChestOpen(chests[i]))
			{
				closeChest(chests[i]);
			}
		}
	}
}

void ChestManager::checkToOpen(sf::FloatRect interacterRect)
{
	//For all chests
	for (int i = 0; i < chests.size(); i++)
	{
		//Grab chest collision box
		sf::FloatRect chestRect = chests[i]->getCollisionBox();
		//Check for collision between player interact box and chest rect
		if (ExtendedCollision::checkCollisionBoxes(&interacterRect, &chestRect))
		{
			//If chest is open close it
			if (isChestOpen(chests[i]))
				closeChest(chests[i]);
			else
				openChest(chests[i]);
			//Play chest opening sound
			audio->playSoundbyName("ChestOpen");
			return;
		}
		else
		{
			if (isChestOpen(chests[i]))
				closeChest(chests[i]);
		}
	}
}

void ChestManager::draw(sf::RenderWindow* window)
{
	for (int i = 0; i < chests.size(); i++)
	{
		window->draw(*chests[i]);
	}
}

void ChestManager::openChest(Chest* chest)
{
	inventoryManager->getChestInventory(chest)->setShowing(true);
}

void ChestManager::closeChest(Chest* chest)
{
	inventoryManager->getChestInventory(chest)->setShowing(false);
}

bool ChestManager::isChestOpen(Chest* chest)
{
	return inventoryManager->getChestInventory(chest)->getShowing();
}

int ChestManager::getIDForChest(Chest* chest)
{
	for (int i = 0; i < chests.size(); i++)
	{
		if (chests[i] == chest)
			return i;
	}
	return -1;
}