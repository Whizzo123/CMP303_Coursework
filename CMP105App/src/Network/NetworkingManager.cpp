#include "NetworkingManager.h"

// Static Initialisers
bool NetworkingManager::_server = false;
bool NetworkingManager::_serverStarted = false;
bool NetworkingManager::_clientStarted = false;
NetworkConnection** NetworkingManager::_connections = new NetworkConnection*[numberOfConnectionsAllowed];
std::map<int, NetworkPlayer*> NetworkingManager::_players = std::map<int, NetworkPlayer*>();
int NetworkingManager::_connectionIndex = 1;
MListener* NetworkingManager::_listener = 0;
MSocket* NetworkingManager::_mySocket = 0;
MSocketSelector* NetworkingManager::_selector = 0;
int NetworkingManager::_myConnectionIndex = 0;
Player* NetworkingManager::localPlayer = nullptr;
std::map<int, bool> NetworkingManager::changeStateOfNetworkObjects = std::map<int, bool>();
std::vector<NetworkObject*> NetworkingManager::networkObjects = std::vector<NetworkObject*>();
GameLevel* NetworkingManager::currentLevel = 0;

bool NetworkingManager::Find()
{
	return true;
}

/// <summary>
/// Starts a server on this computer
/// </summary>
/// <returns></returns>
bool NetworkingManager::StartServer(Level& level, Input* input, sf::RenderWindow* window, AudioManager* audioManager)
{
	_server = true;
	_listener = new MListener(port);
	_selector = new MSocketSelector();
	_listener->listen();
	std::cout << "Listening" << std::endl;
	CreateLocalPlayer(input, window, audioManager);
	MSocket* clientSocket = _listener->accept();
	// Add client to NetworkConnections
	_connections[_connectionIndex] = new NetworkConnection(_connectionIndex, clientSocket);
	_selector->add(_connections[_connectionIndex]->getConnectionSocket());
	// Create a network player for them
	sf::Vector2f spawnPos = sf::Vector2f(100, 275);
	_players[_connectionIndex] = new NetworkPlayer("Dwarf_V2", spawnPos, sf::Vector2f(100, 100), 200.0f, audioManager, _connectionIndex, false);
	std::cout << "Client connected" << std::endl;
	// Send the client a welcome connection
	sf::Packet sendPacket;
	sendPacket << sf::String("Hello this is the server!!!");
	_connections[_connectionIndex]->getConnectionSocket()->send(sendPacket);
	// Send the client connection index to it
	sf::Packet connectionIndexPacket;
	connectionIndexPacket << sf::Int32(_connectionIndex);
	_connections[_connectionIndex]->getConnectionSocket()->send(connectionIndexPacket);
	// Increment connection index
	_connectionIndex++;
	_serverStarted = true;
	level.setNextLevel(2);
	return true;
}

std::vector<sf::Vector2f> NetworkingManager::GetPlayerPos()
{
	std::vector<sf::Vector2f> positions;
	positions.push_back(localPlayer->getPosition());
	for (auto player : _players)
	{
		positions.push_back(player.second->getPosition());
	}
	return positions;
}
void NetworkingManager::SearchForCalls(float dt)
{
	// Use selector to see if any sockets have sent data to be used
	_selector->wait();
	int socketID = _selector->GrabReadySocket(_connections, _connectionIndex);
	std::cout << "Searching" << std::endl;
	if (socketID != -1)
	{
		std::cout << "Recieving a message" << std::endl;
		sf::Packet recievePacket;
		_connections[socketID]->getConnectionSocket()->recieve(recievePacket);
		std::cout << "Recieved message to packet" << std::endl;
		FunctionName recvFuncName;
		recievePacket >> recvFuncName;
		std::string functionName = recvFuncName.funcName;
		std::cout << "Recieved function name is " << functionName << std::endl;
		//FUNCTION CALLS
		if (functionName == "GetPlayerPos")
		{
			std::cout << "Calling function GetPlayerPos" << std::endl;
			sf::Packet sendPacket;
			PlayerPosResult result;
			std::vector<sf::Vector2f> positions = GetPlayerPos();
			result.vecLength = positions.size();
			for (size_t i = 0; i < 2; i++)
			{
				result.resultPositions.push_back(positions[i]);
			}
			sendPacket << result;
			std::cout << "Passing back to client" << std::endl;
			_connections[socketID]->getConnectionSocket()->send(sendPacket);
		}
		else if (functionName == "SyncNetworkPosition")
		{
			std::cout << "Calling function SyncNetworkPosition" << std::endl;
			SyncNetworkPosition(socketID);
		}
		else if (functionName == "GetEnemies")
		{
			std::cout << "Calling function GetEnemies" << std::endl;
			GetEnemyInfoForClient(socketID);
		}
	}
	for (auto player : _players)
	{
		player.second->Update(dt);
	}
}

void NetworkingManager::GetEnemyInfoForClient(int socketID)
{
	std::vector<Enemy*> enemies = currentLevel->getEnemies();
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
		info.patrolPosition = sf::Vector2f(0.0f, 0.0f);
		enemiesInfo[i] = info;
	}
	result.length = enemies.size();
	result.enemiesInfo = enemiesInfo;
	sf::Packet sendEnemyInfoPacket;
	sendEnemyInfoPacket << result;
	_connections[socketID]->getConnectionSocket()->send(sendEnemyInfoPacket);

}

void NetworkingManager::SyncNetworkPosition(int socketID)
{
	//CLIENT ISN'T SYNCING TO SERVER
	NetworkObjectUpdateData updatedObjects;
	sf::Packet syncRecvPacket;
	_connections[socketID]->getConnectionSocket()->recieve(syncRecvPacket);
	syncRecvPacket >> updatedObjects;
	for (int i = 0; i < updatedObjects.length; i++)
	{
		// TODO change this to network objects
		_players[updatedObjects.posSyncVars[i].objectID]->setPosition(updatedObjects.posSyncVars[i].newPosition);
	}
	sf::Packet syncSendPacket;
	PlayerPosResult result;
	std::vector<sf::Vector2f> positions = GetPlayerPos();
	result.vecLength = positions.size();
	for (size_t i = 0; i < 2; i++)
	{
		result.resultPositions.push_back(positions[i]);
	}
	syncSendPacket << result;
	for (int i = 1; i < _connectionIndex; i++)
	{
		_connections[i]->getConnectionSocket()->send(syncSendPacket);
	}
}

void NetworkingManager::ClientNetworkSync(float dt)
{
	//Handle any requested events from server
	sf::Packet packet;
	if (_mySocket->recieve(packet))
	{
		// I have recieved data to update my network objects with
		PlayerPosResult result;
		packet >> result;
		SyncNetworkPlayerPositions(result.resultPositions);
		for (auto player : _players)
		{
			player.second->Update(dt);
		}
	}
	// Send any changed data
	_mySocket->send(PacketUpdatedNetworkObjectData());
	std::cout << "Sending change data" << std::endl;
}

sf::Packet NetworkingManager::PacketUpdatedNetworkObjectData()
{
	sf::Vector2f myPositionToSend = localPlayer->getPosition();
	// Call function
	FunctionName funcToCall;
	funcToCall.funcName = "SyncNetworkPosition";
	sf::Packet funcCallPacket;
	funcCallPacket << funcToCall;
	_mySocket->send(funcCallPacket);
	// Pass parameters
	NetworkObjectUpdateData updateData;
	std::vector<NetworkObject> updatedObjects = GetUpdatedNetworkObjects();
	int updateArrayLength = updatedObjects.size() + 1;
	updateData.length = updateArrayLength;
	NetworkObjectPositionSyncVar* syncVars = new NetworkObjectPositionSyncVar[updateArrayLength];
	for (int i = 0; i < updateData.length - 1; i++)
	{
		NetworkObjectPositionSyncVar var;
		var.objectID = updatedObjects[i].GetID();
		var.newPosition = updatedObjects[i].getPosition();
		syncVars[i] = var;
	}
	// Add on local player position
	NetworkObjectPositionSyncVar posSyncVar;
	posSyncVar.objectID = _myConnectionIndex;
	posSyncVar.newPosition = myPositionToSend;
	syncVars[updateArrayLength - 1] = posSyncVar;
	// Set Update Data to point to sync vars
	updateData.posSyncVars = syncVars;
	sf::Packet posSyncPacket;
	posSyncPacket << updateData;
	return posSyncPacket;
}

std::vector<NetworkObject> NetworkingManager::GetUpdatedNetworkObjects()
{
	std::vector<NetworkObject> changedObjects;
	int index = 0;
	// Loop through the map of networkObject states
	for (auto objectID : changeStateOfNetworkObjects)
	{
		// If this object is recorded as having changed
		if (objectID.second == true)
		{
			NetworkObject* nObject = networkObjects.at(objectID.first);
			changedObjects.push_back(*nObject);
			index++;
		}
	}
	return changedObjects;
}

//TODO Have socket changed to pass back the packet rather than having to make one and then send it 

/// <summary>
/// Starts a client on this computer and joins with server
/// </summary>
/// <returns></returns>
bool NetworkingManager::StartClient(Level& level, Input* input, sf::RenderWindow* window, AudioManager* audioManager)
{
	_server = false;
	sf::IpAddress ip = "127.0.0.1";
	_mySocket = new MSocket(ip, port, false);
	sf::Packet recievePacket;
	_mySocket->recieve(recievePacket);
	sf::String recievedStringData;
	recievePacket >> recievedStringData;
	std::cout << "Recieved data" << recievedStringData.toAnsiString() << std::endl;
	sf::Packet connectionIndexRecvPacket;
	_mySocket->recieve(connectionIndexRecvPacket);
	connectionIndexRecvPacket >> _myConnectionIndex;
	std::cout << "Connection index" << _myConnectionIndex << std::endl;
	CreateLocalPlayer(input, window, audioManager);
	sf::Packet posResultPacket = CallServer("GetPlayerPos");
	PlayerPosResult results; 
	posResultPacket >> results;
	for (int i = 0; i < results.resultPositions.size(); i++)
	{
		if (i != _myConnectionIndex)
		{
			sf::Vector2f spawnPos = results.resultPositions[i];
			_players[i] = new NetworkPlayer("Dwarf_V2", spawnPos, sf::Vector2f(100, 100), 200.0f, audioManager, i, false);
			networkObjects.push_back(_players[i]);
		}
		else
		{
			sf::Vector2f spawnPos = results.resultPositions[i];
			localPlayer->setPosition(spawnPos);
			networkObjects.push_back(new NetworkObject());
		}
	}
	level.setNextLevel(2);
	_mySocket->setBlocking(false);
	_clientStarted = true;
	// Load level
	
	// Spawn players
	return true;
}

void NetworkingManager::SpawnNetworkedEnemies()
{
	_mySocket->setBlocking(true);
	sf::Packet enemySpawnInfoPacket = CallServer("GetEnemies");
	_mySocket->setBlocking(false);
	EnemySpawnInfoResult result;
	enemySpawnInfoPacket >> result;
	currentLevel->spawnInEntities(result.enemiesInfo, result.length);
}

sf::Packet  NetworkingManager::CallServer(sf::String funcName)
{
	// Call this function on server-side
	std::cout << "Sending call to server" << std::endl;
	FunctionName name;
	name.funcName = funcName;
	sf::Packet funcCallPacket;
	funcCallPacket << name;
	_mySocket->send(funcCallPacket);
	// Await function results
	sf::Packet recvPacket;
	_mySocket->recieve(recvPacket);
	std::cout << "Receiving result from server" << std::endl;
	return recvPacket;
}

void NetworkingManager::CreateLocalPlayer(Input* input, sf::RenderWindow* window, AudioManager* audio)
{
	localPlayer = new Player("Dwarf_V2", sf::Vector2f(300, 275), sf::Vector2f(100, 100), 200.0f, input, window, audio, 10.0f);
	Inventory* inventory = new Inventory(window, input);
	inventory->addToSlot(10, new Weapon("Axe of Grimoth", 1, 1, 1.5));
	inventory->addToSlot(0, new Armour("Helmet of Valor", 1, 1, HEAD));
	inventory->addToSlot(1, new Armour("Mail of Fire", 1, 2, CHEST));
	inventory->addToSlot(2, new Armour("Legs of Speed", 1, 1, LEGS));
	inventory->addToSlot(3, new Armour("Skeleton Boots", 1, 1, FEET));
	localPlayer->setInventory(inventory);
}

bool NetworkingManager::JoinServer()
{
	return true;
}

bool NetworkingManager::isServer()
{
	return _server;
}

bool NetworkingManager::isServerStarted()
{
	return _serverStarted;
}

bool NetworkingManager::isClientStarted()
{
	return _clientStarted;
}

std::vector<NetworkPlayer*> NetworkingManager::GetNetworkPlayers()
{
	std::vector<NetworkPlayer*> _returnedPlayers;
	for (auto networkPlayer : _players)
	{
		_returnedPlayers.push_back(networkPlayer.second);
	}
	return _returnedPlayers;
}

void NetworkingManager::SyncNetworkPlayerPositions(std::vector<sf::Vector2f> positions)
{
	for (int i = 0; i < positions.size(); i++)
	{
		if (i != _myConnectionIndex)
		{
			sf::Vector2f pos = positions[i];
			_players[i]->setPosition(pos);
		}
	}
}

void NetworkingManager::AddNetworkObject(NetworkObject* object)
{
	networkObjects.push_back(object);
	changeStateOfNetworkObjects[object->GetID(), false];
}

sf::Packet& operator << (sf::Packet& packet, const PlayerPosResult& results)
{
	packet << results.vecLength;
	for (int i = 0; i < results.vecLength; i++)
	{
		packet << results.resultPositions[i];
	}
	return packet;
}

sf::Packet& operator >> (sf::Packet& packet, PlayerPosResult& results)
{
	packet >> results.vecLength;
	for (int i = 0; i < results.vecLength; i++)
	{
		sf::Vector2f resultPos;
		packet >> resultPos;
		results.resultPositions.push_back(resultPos);
	}
	return packet;
}

sf::Packet& operator << (sf::Packet& packet, const sf::Vector2f& position)
{
	return packet << position.x << position.y;
}

sf::Packet& operator >> (sf::Packet& packet, sf::Vector2f& position)
{
	return packet >> position.x >> position.y;
}

sf::Packet& operator << (sf::Packet& packet, const FunctionName& name)
{
	return packet << name.funcName;
}

sf::Packet& operator >> (sf::Packet& packet, FunctionName& name)
{
	return packet >> name.funcName;
}

sf::Packet& operator << (sf::Packet& packet, const NetworkObjectPositionSyncVar& syncVar)
{
	return packet << syncVar.objectID << syncVar.newPosition;
}

sf::Packet& operator >> (sf::Packet& packet, NetworkObjectPositionSyncVar& syncVar)
{
	return packet >> syncVar.objectID >> syncVar.newPosition;
}

sf::Packet& operator << (sf::Packet& packet, const NetworkObjectUpdateData& data)
{
	packet << data.length;
	for (int i = 0; i < data.length; i++)
	{
		packet << data.posSyncVars[i];
	}
	return packet;
}
sf::Packet& operator >> (sf::Packet& packet, NetworkObjectUpdateData& data)
{
	packet >> data.length;
	data.posSyncVars = new NetworkObjectPositionSyncVar[data.length];
	for (int i = 0; i < data.length; i++)
	{
		NetworkObjectPositionSyncVar resultPos;
		packet >> resultPos;
		data.posSyncVars[i] = resultPos;
	}
	return packet;
}

sf::Packet& operator << (sf::Packet& packet, const EnemySpawnInfoResult& data)
{
	packet << data.length;
	for (int i = 0; i < data.length; i++)
	{
		packet << data.enemiesInfo[i];
	}
	return packet;
}

sf::Packet& operator >> (sf::Packet& packet, EnemySpawnInfoResult& data)
{
	packet >> data.length;
	data.enemiesInfo = new EnemyInfo[data.length];
	for (int i = 0; i < data.length; i++)
	{
		EnemyInfo info;
		packet >> info;
		data.enemiesInfo[i] = info;
	}
	return packet;
}

sf::Packet& operator << (sf::Packet& packet, const EnemyInfo& data)
{
	return packet << data.objectID << data.enemyClass << data.spawnPosition << data.patrolPosition;
}

sf::Packet& operator >> (sf::Packet& packet, EnemyInfo& data)
{
	return packet >> data.objectID >> data.enemyClass >> data.spawnPosition >> data.patrolPosition;
}

sf::Packet& operator << (sf::Packet& packet, const EnemyType& data)
{
	return packet << static_cast<int>(data);
}

sf::Packet& operator >> (sf::Packet& packet, EnemyType& data)
{
	int enumIndex;
	packet >> enumIndex;
	data = static_cast<EnemyType>(enumIndex);
	return packet;
}