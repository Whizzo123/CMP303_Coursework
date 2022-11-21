#include "GameLevel.h"

GameLevel::GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, Player* player,
	int levelIndex, int numberOfEnemies, int numberOfChests, std::vector<int>* mapData, sf::Vector2u mapDimensions) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	this->player = player;
	this->levelIndex = levelIndex;
	inventoryManager = new InventoryManager();
	dragController = new DragController(input, inventoryManager, window);
	chestManager = new ChestManager(input, inventoryManager, audio);
	map = new DungeonDiverTileMap("DungeonWall_V2", sf::Vector2f(75, 75), mapDimensions, *mapData);
	backgroundMap = new BackgroundMap("DungeonWall_V2", sf::Vector2f(75, 75), sf::Vector2u(100, 100));
	// We wannna get rid of this guy for now
	// ____________________________________________
	characterManager = new CharacterManager(numberOfEnemies, audio, map);
	pauseFont.loadFromFile("font/arial.ttf");
	pauseText.setFont(pauseFont);
	pauseText.setString("Paused");
	pauseTextPos = sf::Vector2f(550, 150);
	skipButtonPos = sf::Vector2f(900, 500);
	pauseText.setPosition(pauseTextPos);
	skipLevelButton = new LevelLoaderButton("skipButton", skipButtonPos, sf::Vector2f(150, 75), input, window, 0);
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
		player->handleInput(dt);
		cursor->handleInput();
	}
}

void GameLevel::spawnInEntities()
{
	dungeonExit = new DungeonExit(map->getSpawnableTiles().back(), &nextLevel, levelIndex);
	chestManager->spawn(map, window, input, _numberOfChests);
}

void GameLevel::spawnInEntities(EnemyInfo* info, int enemyInfoLength)
{
	characterManager->spawnNetworkEnemies(info, enemyInfoLength);
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
	if (gameState->getCurrentState() != State::PAUSE)
	{
		//Check for map collisions
		map->collisionCheck(player);
		//Update characters and player
		characterManager->update(dt, map, player);
		player->update(dt);
		for (auto player : _otherPlayers)
		{
			player.second->update(dt);
		}
		player->setEnemyTarget(characterManager->checkCollisions(player->getAttackRect()));
		background->update(dt);
		inventoryManager->update(dt);
		dragController->update(dt);
		healthUI->update();
		cursor->update();		
	}
}

void GameLevel::handleNetwork(float dt)
{
	// Use selector to see if any sockets have sent data to be used
	if (NetworkingManager::isServer())
	{
		int socketID = NetworkingManager::FindReadySockets();
		if (socketID != -1)
		{
			sf::Packet recievePacket = NetworkingManager::RecievePacketOnSocket(socketID);
			std::string functionName;
			recievePacket >> functionName;
			std::cout << "Recieved function name is " << functionName << std::endl;
			//FUNCTION CALLS
			if (functionName == "GetPlayerPos")
				NetworkingManager::SendPlayerPosResultPacket(GetPlayerPos(), socketID);
			else if (functionName == "SyncNetworkPosition")
			{
				if (NetworkingManager::IsCharacterInitialised(socketID))
				{
					NetworkObjectUpdateData data;
					recievePacket >> data;
					SyncNetworkPosition(data);
				}
			}
			else if (functionName == "GetEnemies")
				GetEnemyInfoForClient(socketID);
		}
		if (NetworkingManager::IsCharacterInitialised(socketID))
			ServerUpdateEnemyPositions();
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
			{
				PlayerPosResult result;
				recvPacket >> result;
				SyncNetworkPlayerPositions(result.resultPositions);
			}
			else if (eventName == "ServerUpdateEnemyPositions")
			{
				if (characterManager->getCurrentCharacterCount() > 0)
				{
					NetworkObjectUpdateData data;
					recvPacket >> data;
					SyncNetworkEnemyPositions(data);
				}
			}
			else if (eventName == "SpawnNetworkedEnemies")
			{
				EnemySpawnInfoResult result;
				recvPacket >> result;
				SpawnNetworkedEnemies(result);
			}
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
			//TODO add enemy movement syncing
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
	//window->draw(*dungeonExit);
	characterManager->draw(window);
	player->drawDebugInfo();
	for (auto networkPlayer : _otherPlayers)
	{
		window->draw(*networkPlayer.second);
	}
	if (player->isAlive())
	{
		window->draw(*player);
		player->drawStatIndicator(window);
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

void GameLevel::switchToLevel()
{
	setPlayer(NetworkingManager::localPlayer);
	_otherPlayers = NetworkingManager::GetNetworkPlayers();
	inventoryManager->addToInventories(player->getInventory());
	healthUI = new HealthUI(player, window, sf::Vector2f(window->getSize().x - 150.0f, window->getSize().y - 40.0f));
	cursor = new Cursor(window, inventoryManager, input, player);
	background = new Background(window, player);
	player->setChestManager(chestManager);
	player->setDungeonExit(dungeonExit);
	player->setNextLevel(&nextLevel);
	if (NetworkingManager::isServer())
	{
		characterManager->spawnAllCharacters();
	}
	else
	{
		NetworkingManager::SendFunctionCall("GetEnemies");
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
	positions.push_back(player->getPosition());
	for (auto player : _otherPlayers)
	{
		positions.push_back(player.second->getPosition());
	}
	return positions;
}

void GameLevel::SyncNetworkPosition(NetworkObjectUpdateData updatedObjects)
{
	for (int i = 0; i < updatedObjects.playerLength; i++)
		_otherPlayers[updatedObjects.playerPosSyncVars[i].objectID]->setPosition(updatedObjects.playerPosSyncVars[i].newPosition);
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
		// TODO switch out this connectionIndex for something else
		info.objectID = i;
		info.enemyClass = enemies[i]->getEnemyType();
		info.spawnPosition = enemies[i]->getPosition();
		info.patrolPosition = enemies[i]->getMoveDirection();
		enemiesInfo[i] = info;
	}
	NetworkingManager::SendEnemySpawnInfoResult(enemiesInfo, length, socketID);
	NetworkingManager::SetCharacterInitialised(socketID);
}

void GameLevel::ServerUpdateEnemyPositions()
{
	// Grab all target positions of enemies
	std::vector<sf::Vector2f> enemyPositions = characterManager->getEnemyTargetPositions();
	int length = enemyPositions.size();
	NetworkObjectPositionSyncVar* syncVars = new NetworkObjectPositionSyncVar[length];
	for (int i = 0; i < length; i++)
	{
		NetworkObjectPositionSyncVar syncVar;
		syncVar.objectID = i;
		syncVar.newPosition = enemyPositions[i];
		std::cout << "Enemy Patrol Pos: " << syncVar.newPosition.x << "," << syncVar.newPosition.y << std::endl;
		syncVars[i] = syncVar;
	}
	NetworkObjectUpdateData data;
	data.enemyLength = length;
	data.playerLength = 0;
	data.enemyPosSyncVars = syncVars;
	data.playerPosSyncVars = 0;
	// Call client-side function
	for (int i = 1; i < NetworkingManager::GetNumConnections(); i++)
	{
		if(NetworkingManager::IsCharacterInitialised(i))
			NetworkingManager::SendUpdatedNetworkData("ServerUpdateEnemyPositions", data, i);
	}
}

///CLIENT FUNCTIONS////////////////////////////////////////////////
void GameLevel::SyncNetworkPlayerPositions(std::vector<sf::Vector2f> positions)
{
	for (int i = 0; i < positions.size(); i++)
	{
		sf::Vector2f pos = positions[i];
		if (i != NetworkingManager::GetMyConnectionIndex())
		{
			_otherPlayers[i]->setPosition(pos);
		}
	}
}

void GameLevel::PacketUpdatedNetworkObjectData()
{
	// Pass parameters
	NetworkingManager::SendUpdatedNetworkData(GetUpdatedNetworkObjects(), player);
}

//TODO this won't work players are no longer network objects 
std::vector<NetworkObject> GameLevel::GetUpdatedNetworkObjects()
{
	std::vector<NetworkObject> changedObjects;
	int index = 0;
	// Loop through the map of networkObject states
	for (auto objectID : NetworkingManager::GetNObjectChangeState())
	{
		// If this object is recorded as having changed
		if (objectID.second == true)
		{
			NetworkObject* nObject = NetworkingManager::GetPlayerNetworkObjects().at(objectID.first);
			changedObjects.push_back(*nObject);
			index++;
		}
	}
	return changedObjects;
}

void GameLevel::SpawnNetworkedEnemies(EnemySpawnInfoResult result)
{
	spawnInEntities(result.enemiesInfo, result.length);
}

void GameLevel::SyncNetworkEnemyPositions(NetworkObjectUpdateData data)
{
	NetworkObjectPositionSyncVar* syncVars = data.enemyPosSyncVars;
	for (int i = 0; i < data.enemyLength; i++)
	{
		std::cout << "Client enemyPos " << i << ": " << syncVars[i].newPosition.x << "," << syncVars[i].newPosition.y << std::endl;
		characterManager->getEnemy(i)->setMoveDirection(syncVars[i].newPosition);
	}
}