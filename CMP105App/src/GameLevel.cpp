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
			std::cout << "Recieving a message" << std::endl;
			sf::Packet recievePacket = NetworkingManager::RecievePacketOnSocket(socketID);
			std::cout << "Recieved message to packet" << std::endl;
			FunctionName recvFuncName;
			recievePacket >> recvFuncName;
			std::string functionName = recvFuncName.funcName;
			std::cout << "Recieved function name is " << functionName << std::endl;
			//FUNCTION CALLS
			if (functionName == "GetPlayerPos")
			{
				NetworkingManager::SendPlayerPosResultPacket(GetPlayerPos(), socketID);
			}
			else if (functionName == "SyncNetworkPosition")
			{
				if (NetworkingManager::IsCharacterInitialised(socketID))
				{
					SyncNetworkPosition(socketID);
				}
			}
			else if (functionName == "GetEnemies")
			{
				GetEnemyInfoForClient(socketID);
			}
		}
	}
	else
	{
		//Handle any requested events from server
		sf::Packet packet = NetworkingManager::RecievePacketOnSocket();
		if (packet)
		{
			FunctionName eventCall;
			packet >> eventCall;
			if (eventCall.funcName == "SyncNetworkPlayerPositions")
			{
				std::cout << "Calling client event: SyncNetworkPlayerPositions" << std::endl;
				// I have recieved data to update my network objects with
				sf::Packet playerPosPacket = NetworkingManager::RecievePacketOnSocket();
				PlayerPosResult result;
				playerPosPacket >> result;
				SyncNetworkPlayerPositions(result.resultPositions);
				std::cout << "Finished SyncNetworkPlayerPositions" << std::endl;
			}
			else if (eventCall.funcName == "ServerUpdateEnemyPositions")
			{
				std::cout << "Calling client event: ServerUpdateEnemyPositions" << std::endl;
				if (characterManager->getCurrentCharacterCount() > 0)
				{
					sf::Packet enemyPosPacket = NetworkingManager::RecievePacketOnSocket();
					NetworkObjectUpdateData data;
					enemyPosPacket >> data;
					SyncNetworkEnemyPositions(data);
					std::cout << "Finished ServerUpdateEnemyPositions" << std::endl;
				}
			}
			else
			{
				std::string call = eventCall.funcName.toAnsiString();
				std::cout << "Unhandled event call: " << call << std::endl;
			}
		}
		// Send any changed data
		if (characterManager->getCurrentCharacterCount() > 0)
		{
			std::cout << "PacketingUpdateNetworkObjectData" << std::endl;
			//TODO add enemy movement syncing
			PacketUpdatedNetworkObjectData();
			std::cout << "Sending change data" << std::endl;
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
		NetworkingManager::SetToBlock(true);
		sf::Packet packet = NetworkingManager::RecievePacketOnSocket();
		std::cout << "Recieved" << std::endl;
		EnemySpawnInfoResult result;
		packet >> result;
		SpawnNetworkedEnemies(result);
		NetworkingManager::SetToBlock(false);
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

void GameLevel::SyncNetworkPosition(int socketID)
{
	NetworkObjectUpdateData updatedObjects;
	sf::Packet syncRecvPacket = NetworkingManager::RecievePacketOnSocket(socketID);
	syncRecvPacket >> updatedObjects;
	for (int i = 0; i < updatedObjects.length; i++)
		_otherPlayers[updatedObjects.posSyncVars[i].objectID]->setPosition(updatedObjects.posSyncVars[i].newPosition);
	for (int i = 1; i < NetworkingManager::GetNumConnections(); i++)
	{
		std::vector<sf::Vector2f> playerPositions = GetPlayerPos();
		for (int i = 0; i < playerPositions.size(); i++)
		{
			std::cout << "Pos: " << i << " is: " << playerPositions[i].x << ", " << playerPositions[i].y << std::endl;
		}
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
		syncVar.newPosition = syncVar.newPosition;
		syncVars[i] = syncVar;
	}
	NetworkObjectUpdateData data;
	data.length = length;
	data.posSyncVars = syncVars;
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
		std::cout << "Pos: " << i << " is: " << pos.x << ", " << pos.y << std::endl;
		if (i != NetworkingManager::GetMyConnectionIndex())
		{
			_otherPlayers[i]->setPosition(pos);
		}
	}
}

void GameLevel::PacketUpdatedNetworkObjectData()
{
	// Call function
	NetworkingManager::SendFunctionCall("SyncNetworkPosition");
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
	NetworkObjectPositionSyncVar* syncVars = data.posSyncVars;
	for (int i = 0; i < data.length; i++)
	{
		characterManager->getEnemy(i)->setMoveDirection(syncVars[i].newPosition);
	}
}