#include "GameLevel.h"

GameLevel::GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, Player* player,
	sf::Vector2f spawnPoint, int levelIndex, int numberOfEnemies, int numberOfChests, std::vector<int>* mapData, sf::Vector2u mapDimensions) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	//this->player = player;
	this->levelIndex = levelIndex;
	inventoryManager = new InventoryManager();
	dragController = new DragController(input, inventoryManager, window);
	chestManager = new ChestManager(input, inventoryManager, audio);
	map = new DungeonDiverTileMap("DungeonWall_V2", sf::Vector2f(75, 75), mapDimensions, *mapData);
	backgroundMap = new BackgroundMap("DungeonWall_V2", sf::Vector2f(75, 75), sf::Vector2u(100, 100));
	dungeonExit = new DungeonExit(map->getSpawnableTiles().back(), &nextLevel, levelIndex);
	characterManager = new CharacterManager(numberOfEnemies, audio, map);
	pauseFont.loadFromFile("font/arial.ttf");
	pauseText.setFont(pauseFont);
	pauseText.setString("Paused");
	pauseTextPos = sf::Vector2f(550, 150);
	skipButtonPos = sf::Vector2f(900, 500);
	pauseText.setPosition(pauseTextPos);
	skipLevelButton = new LevelLoaderButton("skipButton", skipButtonPos, sf::Vector2f(150, 75), input, window, 0);
	_numberOfChests = numberOfChests;
	_spawnPoint = spawnPoint;
}

void GameLevel::handleInput(float dt)
{
	//Handle pause menu 
	skipLevelButton->handleInput(dt);
	if (input->isPressed(sf::Keyboard::Tab))
	{
		if (gameState->getCurrentState() != State::PAUSE)
			gameState->setCurrentState(State::PAUSE);
		else
			gameState->setCurrentState(State::LEVEL);
	}
	//Handle player and cursor input
	if (gameState->getCurrentState() != State::PAUSE)
	{
		for (auto player : _players)
		{
			player.second->handleInput(dt);
		}
		cursor->handleInput();
	}
}

void GameLevel::update(float dt)
{
	if (currentNetworkTickTime > 0.1f)
	{
		handleNetwork(dt);
		currentNetworkTickTime = 0.0f;
	}
	else
		currentNetworkTickTime += dt;
	skipLevelButton->update(dt);
	//If skip level button clicked set next level 
	if (skipLevelButton->isClicked())
	{
		nextLevel = levelIndex + 1;
		skipLevelButton->reset();
	}
	//Set pause menu elements positions with respect to view position
	pauseText.setPosition(pauseTextPos + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
	skipLevelButton->setPosition(skipButtonPos + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
	// Normal game update loop stuff
	if (gameState->getCurrentState() != State::PAUSE)
	{
		//Check for map collisions
		for (auto player : _players)
		{
			if (player.first == NetworkingManager::GetMyConnectionIndex() && player.second->isAlive() == false)
			{
				HandlePlayerDeath(player.first);
				std::cout << "Player Death" << std::endl;
			}
			map->collisionCheck(player.second);
			//Update characters and player
			characterManager->update(dt, map, player.second);
			player.second->update(dt);
			player.second->setEnemyTarget(characterManager->checkCollisions(player.second->getAttackRect()));
		}
		background->update(dt);
		inventoryManager->update(dt);
		dragController->update(dt);
		healthUI->update();
		cursor->update();		
	}
}

void GameLevel::handleNetwork(float dt)
{
	// Check whether we are the server or a client
	if (NetworkingManager::isServer())
	{
		// Are there any ready sockets?
		int socketID = NetworkingManager::FindReadySockets();
		while (socketID != -1)
		{
			sf::Packet* packet = NetworkingManager::RecievePacketOnSocket(socketID);
			if (packet)
			{
				sf::Packet recievePacket = *packet;
				std::string functionName;
				recievePacket >> functionName;
				//FUNCTION CALLS
				if (functionName == "GetPlayerPos")
				{
					NetworkingManager::SendPlayerPosResultPacket(GetPlayerPos(), socketID);
				}
				else if (functionName == "SyncNetworkPosition")
				{
					if (NetworkingManager::IsCharacterInitialised(socketID))
						SyncNetworkPosition(recievePacket);
				}
				else if (functionName == "GetEnemies")
					GetEnemyInfoForClient(socketID);
				else if (functionName == "GetChests")
					GetChestInfoForClient(socketID);
				else if (functionName == "SyncPlayerAttackedEvent")
					SyncPlayerAttackedEvent(recievePacket);
				else if (functionName == "SyncPlayerReviveEvent")
					SyncPlayerReviveEvent(recievePacket);
				else if (functionName == "SyncInventoryChangeEvent")
					SyncInventoryChangeEvent(recievePacket);
				else if (functionName == "NextLevel")
				{
					dungeonExit->loadLevel();
					NetworkingManager::SendNextLevelMessage();
				}
				else
					std::cout << "Unhandled event call" << functionName << std::endl;
			}
			socketID = NetworkingManager::FindReadySockets();
		}
		
		for (int i = 1; i < NetworkingManager::GetNumConnections(); i++)
		{
			if (NetworkingManager::IsCharacterInitialised(i))
			{
				ServerUpdateEnemyPositions();
			}
		}
	}
	else
	{
		//Handle any requested events from server
		sf::Packet* packet = NetworkingManager::RecievePacketOnSocket();
		while (packet)
		{
			sf::Packet recvPacket = *packet;
			std::string eventName;
			recvPacket >> eventName;
			if (eventName == "SyncNetworkPlayerPositions")
				SyncNetworkPlayerPositions(recvPacket);
			else if (eventName == "ServerUpdateEnemyPositions")
			{
				if (characterManager->getCurrentCharacterCount() > 0)
					SyncNetworkEnemyPositions(recvPacket);
			}
			else if (eventName == "SpawnNetworkedEnemies")
				SpawnNetworkedEnemies(recvPacket);
			else if (eventName == "SpawnNetworkedChests")
				SpawnNetworkedChests(recvPacket);
			else if (eventName == "PlayerAttackedEvent")
			{
				PlayerAttackData data;
				recvPacket >> data;
				HandlePlayerAttackedEvent(data);
			}
			else if (eventName == "PlayerReviveEvent")
			{
				int playerID;
				sf::Vector2f pos;
				recvPacket >> playerID;
				recvPacket >> pos;
				HandlePlayerReviveEvent(playerID, pos);
			}
			else if (eventName == "InventoryChangeEvent")
			{
				InventorySyncData data;
				recvPacket >> data;
				HandleInventoryChangeEvent(data);
			}
			else if (eventName == "NextLevel")
				dungeonExit->loadLevel();
			else
			{
				std::string call = eventName;
				std::cout << "Unhandled event call: " << call << std::endl;
			}
			packet = NetworkingManager::RecievePacketOnSocket();
		}
		// Send any changed data
		if (characterManager->getCurrentCharacterCount() > 0)
		{
			PacketUpdatedNetworkObjectData();
		}
	}
}

void GameLevel::render()
{
	beginDraw();
	backgroundMap->render(window);
	map->render(window);
	chestManager->draw(window);
	window->draw(*dungeonExit);
	characterManager->draw(window);
	for (auto networkPlayer : _players)
	{
		if (networkPlayer.second->isAlive())
		{
			window->draw(*networkPlayer.second);
			networkPlayer.second->drawStatIndicator(window);
		}
	}
	inventoryManager->draw(window);
	healthUI->draw();
	cursor->draw();
	if (gameState->getCurrentState() == State::PAUSE)
	{
		window->draw(pauseText);
		window->draw(*skipLevelButton);
	}
	endDraw();
}

void GameLevel::switchToLevel(std::map<int, Player*> players)
{
	_players = players;
	for (int i = 0; i < _players.size(); i++)
	{
		inventoryManager->addToInventories(_players[i]->getInventory());
	}
	healthUI = new HealthUI(_players[NetworkingManager::GetMyConnectionIndex()], window, sf::Vector2f(window->getSize().x - 150.0f, window->getSize().y - 40.0f));
	cursor = new Cursor(window, inventoryManager, input, _players[NetworkingManager::GetMyConnectionIndex()]);
	background = new Background(window, _players[NetworkingManager::GetMyConnectionIndex()]);
	sf::Vector2f spawnPos = sf::Vector2f(rand() % ((int)_spawnPoint.x + 100) + ((int)_spawnPoint.x - 100), (int)_spawnPoint.y);
	_players[NetworkingManager::GetMyConnectionIndex()]->setPlayerPosition(spawnPos);
	_players[NetworkingManager::GetMyConnectionIndex()]->setChestManager(chestManager);
	_players[NetworkingManager::GetMyConnectionIndex()]->setDungeonExit(dungeonExit);
	_players[NetworkingManager::GetMyConnectionIndex()]->setNextLevel(&nextLevel);
	if (NetworkingManager::isServer())
	{
		characterManager->spawnAllCharacters();
		chestManager->spawn(map, window, input, _numberOfChests);
	}
	else
	{
		NetworkingManager::SendFunctionCall("GetEnemies");
		NetworkingManager::SendFunctionCall("GetChests");
	}
}


///NETWORKING FUNCTIONALLITY/////////////////////////////////////

/// <summary>
/// Gets all player positions to a vector
/// </summary>
/// <returns></returns>
std::vector<sf::Vector2f> GameLevel::GetPlayerPos()
{
	std::vector<sf::Vector2f> positions;
	for (auto player : _players)
	{
		positions.push_back(player.second->getPosition());
	}
	return positions;
}

void GameLevel::SyncNetworkPosition(sf::Packet packet)
{
	NetworkObjectUpdateData updatedObjects;
	packet >> updatedObjects;
	for (int i = 0; i < updatedObjects.playerLength; i++)
	{
		sf::Vector2f pos = updatedObjects.playerPosSyncVars[i].newPosition;
		_players[updatedObjects.playerPosSyncVars[i].objectID]->updateVelocity(pos, NetworkingManager::GetCurrentTime());
	}
	for (int i = 1; i < NetworkingManager::GetNumConnections(); i++)
	{
		std::vector<sf::Vector2f> playerPositions = GetPlayerPos();
		NetworkingManager::SendPlayerPosResultPacket(playerPositions, i);
	}
}

void GameLevel::GetEnemyInfoForClient(int socketID)
{
	std::vector<Enemy*> enemies = getEnemies();
	EnemySpawnInfoResult result;
	int length = enemies.size();
	EnemyInfo* enemiesInfo = new EnemyInfo[length];
	for (int i = 0; i < length; i++)
	{
		EnemyInfo info;
		info.objectID = i;
		info.enemyClass = enemies[i]->getEnemyType();
		info.spawnPosition = enemies[i]->getPosition();
		info.patrolPosition = enemies[i]->getMoveDirection();
		enemiesInfo[i] = info;
	}
	NetworkingManager::SendEnemySpawnInfoResult(enemiesInfo, length, socketID);
	NetworkingManager::SetCharacterInitialised(socketID);
}

void GameLevel::GetChestInfoForClient(int socketID)
{
	std::vector<Chest*> chests = chestManager->getChests();
	ChestSpawnInfoResult result;
	int length = chests.size();
	ChestSpawnInfo* chestsInfo = new ChestSpawnInfo[length];
	for (int i = 0; i < length; i++)
	{
		ChestSpawnInfo info;
		Inventory* inventory = inventoryManager->getChestInventory(chests[i]);
		info.position = chests[i]->getPosition();
		info.itemsCount = inventory->getNumOfOccupiedSlots();
		info.items = new ItemData[info.itemsCount];
		for (int j = 0; j < info.itemsCount; j++)
		{
			std::string itemName = inventory->getSlot(j)->getItem()->getItemName();
			info.items[j].itemIndex = ConfigLoader::getItemID(itemName);
			info.items[j].itemType = (int)inventory->getSlot(j)->getItem()->getItemType();
		}
		chestsInfo[i] = info;
	}
	result.chestsInfo = chestsInfo;
	result.length = length;
	NetworkingManager::SendChestSpawnInfoResult(result, socketID);
}

void GameLevel::ServerUpdateEnemyPositions()
{
	// Grab all target positions of enemies
	std::vector<sf::Vector2f> enemyTargetPositions = characterManager->getEnemyTargetPositions();
	std::vector<sf::Vector2f> enemyPositions = characterManager->getEnemyPositions();
	std::vector<Player*> enemyTargets = characterManager->getEnemyFollowingTargets();
	int length = enemyTargetPositions.size();
	EnemyNetworkObject* enemyNetworkObjects = new EnemyNetworkObject[length];
	for (int i = 0; i < length; i++)
	{
		EnemyNetworkObject enemy;
		enemy.objectID = i;
		enemy.position = enemyPositions[i];
		enemy.velocity = enemyTargetPositions[i];
		enemy.targetedPlayerID = GetIndexForPlayer(enemyTargets[i]);
		enemyNetworkObjects[i] = enemy;
	}
	NetworkObjectUpdateData data;
	data.enemyLength = length;
	data.enemyNetworkObjects = enemyNetworkObjects;
	data.playerLength = 0;
	data.playerPosSyncVars = 0;
	// Call client-side function
	for (int i = 1; i < NetworkingManager::GetNumConnections(); i++)
	{
		NetworkingManager::SendUpdatedNetworkData("ServerUpdateEnemyPositions", data, i);
	}
}

void GameLevel::SyncPlayerAttackedEvent(sf::Packet packet)
{
	PlayerAttackData data;
	packet >> data;
	HandlePlayerAttackedEvent(data);
	NetworkingManager::SendPlayerAttackData(data);
}

void GameLevel::SyncPlayerReviveEvent(sf::Packet packet)
{
	int playerID;
	sf::Vector2f pos;
	packet >> playerID;
	packet >> pos;
	HandlePlayerReviveEvent(playerID, pos);
	NetworkingManager::SendPlayerReviveData(playerID, pos);
}

void GameLevel::SyncInventoryChangeEvent(sf::Packet packet)
{
	InventorySyncData data;
	packet >> data;
	HandleInventoryChangeEvent(data);
	NetworkingManager::SendInventorySyncData(data);
}

///CLIENT FUNCTIONS////////////////////////////////////////////////
void GameLevel::SyncNetworkPlayerPositions(sf::Packet packet)
{
	PlayerPosResult result;
	packet >> result;
	for (int i = 0; i < result.resultPositions.size(); i++)
	{
		sf::Vector2f pos = result.resultPositions[i];
		if (i != NetworkingManager::GetMyConnectionIndex())
		{
			_players[i]->updateVelocity(pos, NetworkingManager::GetCurrentTime());
		}
	}
}

void GameLevel::PacketUpdatedNetworkObjectData()
{
	// Pass parameters
	NetworkingManager::SendUpdatedNetworkData(GetUpdatedNetworkObjects());
}

std::vector<NetworkObject> GameLevel::GetUpdatedNetworkObjects()
{
	std::vector<NetworkObject> changedObjects;
	for (auto nObject : NetworkingManager::GetPlayerNetworkObjects())
	{
		if (nObject.second->canUpdate())
		{
			nObject.second->setPosition(_players[nObject.first]->getPosition());
			changedObjects.push_back(*nObject.second);
		}
	}
	return changedObjects;
}

void GameLevel::SpawnNetworkedEnemies(sf::Packet packet)
{
	EnemySpawnInfoResult result;
	packet >> result;
	characterManager->spawnNetworkEnemies(result.enemiesInfo, result.length);
}

void GameLevel::SpawnNetworkedChests(sf::Packet packet)
{
	ChestSpawnInfoResult result;
	packet >> result;
	chestManager->spawnNetworkedChests(result, window, input);
}

void GameLevel::SyncNetworkEnemyPositions(sf::Packet packet)
{
	NetworkObjectUpdateData data;
	packet >> data;
	EnemyNetworkObject* enemyNetworkObjects = data.enemyNetworkObjects;
	for (int i = 0; i < data.enemyLength; i++)
	{
		EnemyNetworkObject enemy = enemyNetworkObjects[i];
		characterManager->getEnemy(i)->setMoveDirection(enemy.velocity);
		characterManager->getEnemy(i)->setPosition(enemy.position);
		characterManager->getEnemy(i)->setFollowingTarget(GetPlayerFromIndex(enemy.targetedPlayerID));
	}
}

int GameLevel::GetIndexForPlayer(Player* player)
{
	if (player)
	{
		std::map<int, Player*>::iterator it;
		for (it = _players.begin(); it != _players.end(); it++)
		{
			if (it->second == player)
				return it->first;
		}
	}
	return -1;		
}

Player* GameLevel::GetPlayerFromIndex(int playerID)
{
	std::map<int, Player*>::iterator it;
	for (it = _players.begin(); it != _players.end(); it++)
	{
		if (it->first == playerID)
			return it->second;
	}
	return nullptr;
}

void GameLevel::HandlePlayerAttackedEvent(PlayerAttackData data)
{
	_players[data.playerID]->setEnemyTarget(characterManager->getEnemy(data.enemyID));
	// Damage is one even with new item in the slot
	int damage = dynamic_cast<Weapon*>(_players[data.playerID]->getInventory()->getSlot(10)->getItem())->getDamage();
	_players[data.playerID]->manualAttack(damage);
}

void GameLevel::HandlePlayerDeath(int playerID)
{
	std::cout << "Handling player death" << std::endl;
	sf::Vector2f pos = sf::Vector2f(rand() % 500 + 200, 275);
	_players[playerID]->setPlayerPosition(pos);
	_players[playerID]->resetHealth();
	std::cout << "Player Health " << playerID << ": " << _players[playerID]->getCurrentHealth();
	NetworkingManager::SendPlayerReviveData(playerID, pos);
}

void GameLevel::HandlePlayerReviveEvent(int playerID, sf::Vector2f pos)
{
	_players[playerID]->setPlayerPosition(pos);
	_players[playerID]->resetHealth();
	_players[playerID]->switchAnimToIdle();
}

void GameLevel::HandleInventoryChangeEvent(InventorySyncData data)
{
	//Take item from inventory
	Inventory* inv = inventoryManager->getInventoryFromID(data.invID);
	Slot* slot = inv->getSlot(data.slotID);
	Item* item = slot->getItem();
	//Move item to player slot
	Inventory* otherInv = inventoryManager->getInventoryFromID(data.otherInvID);
	if (otherInv->getSlot(data.otherInvSlotID)->getItem() != nullptr)
	{
		Item* temp = otherInv->getSlot(data.otherInvSlotID)->getItem();
		if (dynamic_cast<Weapon*>(item))
			otherInv->addToSlot(data.otherInvSlotID, new Weapon(*(dynamic_cast<Weapon*>(item))));
		//If holding item is a potion
		if (dynamic_cast<Potion*>(item))
			otherInv->addToSlot(data.otherInvSlotID, new Potion(*(dynamic_cast<Potion*>(item))));
		//If holdingItem is a piece of armour
		if (dynamic_cast<Armour*>(item))
			otherInv->addToSlot(data.otherInvSlotID, new Armour(*(dynamic_cast<Armour*>(item))));

		if (dynamic_cast<Weapon*>(temp))
			inv->addToSlot(data.slotID, new Weapon(*(dynamic_cast<Weapon*>(temp))));
		//If holding item is a potion
		if (dynamic_cast<Potion*>(temp))
			inv->addToSlot(data.slotID, new Potion(*(dynamic_cast<Potion*>(temp))));
		//If holdingItem is a piece of armour
		if (dynamic_cast<Armour*>(temp))
			inv->addToSlot(data.slotID, new Armour(*(dynamic_cast<Armour*>(temp))));
	}
	else
	{
		if (dynamic_cast<Weapon*>(item))
			otherInv->addToSlot(data.otherInvSlotID, new Weapon(*(dynamic_cast<Weapon*>(item))));
		//If holding item is a potion
		if (dynamic_cast<Potion*>(item))
			otherInv->addToSlot(data.otherInvSlotID, new Potion(*(dynamic_cast<Potion*>(item))));
		//If holdingItem is a piece of armour
		if (dynamic_cast<Armour*>(item))
			otherInv->addToSlot(data.otherInvSlotID, new Armour(*(dynamic_cast<Armour*>(item))));
		slot->clearItem();
	}
}