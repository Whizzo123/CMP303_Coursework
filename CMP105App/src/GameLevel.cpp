#include "GameLevel.h"

GameLevel::GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, Player* player,
	int levelIndex, int numberOfEnemies, int numberOfChests, std::vector<int>* mapData, sf::Vector2u mapDimensions) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	//this->player = player;
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
	if (gameState->getCurrentState() != State::PAUSE)
	{
		//Check for map collisions
		for (auto player : _players)
		{
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
					SyncNetworkPosition(recievePacket);
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
				SyncNetworkPlayerPositions(recvPacket);
			else if (eventName == "ServerUpdateEnemyPositions")
			{
				if (characterManager->getCurrentCharacterCount() > 0)
					SyncNetworkEnemyPositions(recvPacket);
			}
			else if (eventName == "SpawnNetworkedEnemies")
				SpawnNetworkedEnemies(recvPacket);
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
	//player.second->drawDebugInfo();
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

void GameLevel::switchToLevel()
{
	//setPlayer(NetworkingManager::localPlayer);
	_players = NetworkingManager::GetNetworkPlayers();
	inventoryManager->addToInventories(_players[NetworkingManager::GetMyConnectionIndex()]->getInventory());
	healthUI = new HealthUI(_players[NetworkingManager::GetMyConnectionIndex()], window, sf::Vector2f(window->getSize().x - 150.0f, window->getSize().y - 40.0f));
	cursor = new Cursor(window, inventoryManager, input, _players[NetworkingManager::GetMyConnectionIndex()]);
	background = new Background(window, _players[NetworkingManager::GetMyConnectionIndex()]);
	_players[NetworkingManager::GetMyConnectionIndex()]->setChestManager(chestManager);
	_players[NetworkingManager::GetMyConnectionIndex()]->setDungeonExit(dungeonExit);
	_players[NetworkingManager::GetMyConnectionIndex()]->setNextLevel(&nextLevel);
	if (NetworkingManager::isServer())
		characterManager->spawnAllCharacters();
	else
		NetworkingManager::SendFunctionCall("GetEnemies");
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
		_players[updatedObjects.playerPosSyncVars[i].objectID]->setPosition(updatedObjects.playerPosSyncVars[i].newPosition);
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
		if(NetworkingManager::IsCharacterInitialised(i))
			NetworkingManager::SendUpdatedNetworkData("ServerUpdateEnemyPositions", data, i);
	}
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
			_players[i]->setPosition(pos);
	}
}

void GameLevel::PacketUpdatedNetworkObjectData()
{
	// Pass parameters
	NetworkingManager::SendUpdatedNetworkData(GetUpdatedNetworkObjects());
}

//TODO this won't work players are no longer network objects 
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