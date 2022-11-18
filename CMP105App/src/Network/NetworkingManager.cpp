#include "NetworkingManager.h"

// Static Initialisers
bool NetworkingManager::_server = false;
bool NetworkingManager::_serverStarted = false;
bool NetworkingManager::_clientStarted = false;
NetworkConnection** NetworkingManager::_connections = new NetworkConnection*[numberOfConnectionsAllowed];
std::map<int, Player*> NetworkingManager::_players = std::map<int, Player*>();
int NetworkingManager::_connectionIndex = 1;
MListener* NetworkingManager::_listener = 0;
MSocket* NetworkingManager::_mySocket = 0;
MSocketSelector* NetworkingManager::_selector = 0;
int NetworkingManager::_myConnectionIndex = 0;
Player* NetworkingManager::localPlayer = nullptr;
std::map<int, bool> NetworkingManager::changeStateOfNetworkObjects = std::map<int, bool>();
std::vector<NetworkObject*> NetworkingManager::networkObjects = std::vector<NetworkObject*>();
std::map<int, NetworkObject*> NetworkingManager::_playerNetworkObjects = std::map<int, NetworkObject*>();
std::map<int, bool> NetworkingManager::socketIDToCharacterInitialised = std::map<int, bool>();

bool NetworkingManager::Find()
{
	return true;
}

/// <summary>
/// Starts a server on this computer
/// </summary>
/// <returns></returns>
bool NetworkingManager::StartServer(Input* input, sf::RenderWindow* window, AudioManager* audioManager)
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
	_players[_connectionIndex] = new Player("Dwarf_V2", spawnPos, sf::Vector2f(100, 100), 200.0f, nullptr, window, audioManager, 10.0f);
	_playerNetworkObjects[_connectionIndex] = new NetworkObject(_connectionIndex, false);
	socketIDToCharacterInitialised[_connectionIndex] = false;
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
	return true;
}

/// <summary>
/// Starts a client on this computer and joins with server
/// </summary>
/// <returns></returns>
bool NetworkingManager::StartClient(Input* input, sf::RenderWindow* window, AudioManager* audioManager)
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
	SendFunctionCall("GetPlayerPos");
	// TODO create simpler method of dealing with unneccessary functionCall
	sf::Packet* packet = RecievePacketOnSocket();
	sf::Packet posResultPacket = *packet;
	std::cout << "Recieved player pos" << std::endl;
	PlayerPosResult results;
	std::string discardFuncCall;
	posResultPacket >> discardFuncCall;
	posResultPacket >> results;
	for (int i = 0; i < results.resultPositions.size(); i++)
	{
		if (i != _myConnectionIndex)
		{
			sf::Vector2f spawnPos = results.resultPositions[i];
			_players[i] = new Player("Dwarf_V2", spawnPos, sf::Vector2f(100, 100), 200.0f, nullptr, window, audioManager, 10.0f);
			_playerNetworkObjects[i] = new NetworkObject(i, false);
			networkObjects.push_back(_playerNetworkObjects[i]);
		}
		else
		{
			sf::Vector2f spawnPos = results.resultPositions[i];
			localPlayer->setPosition(spawnPos);
			networkObjects.push_back(new NetworkObject());
		}
	}
	_mySocket->setBlocking(false);
	_clientStarted = true;
	return true;
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

int NetworkingManager::FindReadySockets()
{
	_selector->wait();
	int socketID = _selector->GrabReadySocket(_connections, _connectionIndex);
	std::cout << "Searching" << std::endl;
	return socketID;
}

sf::Packet NetworkingManager::RecievePacketOnSocket(int socketID)
{
	sf::Packet recievePacket;
	_connections[socketID]->getConnectionSocket()->recieve(recievePacket);
	return recievePacket;
}

sf::Packet* NetworkingManager::RecievePacketOnSocket()
{
	sf::Packet* packet = new sf::Packet();
	bool okay = _mySocket->recieve(*packet);
	if (okay)
		return packet;
	else
		return nullptr;
}

void NetworkingManager::SendPlayerPosResultPacket(std::vector<sf::Vector2f> positions, int socketID)
{
	sf::Packet sendPacket;
	PlayerPosResult result;
	result.vecLength = positions.size();
	result.resultPositions = positions;
	sendPacket << "SyncNetworkPlayerPositions";
	sendPacket << result;
	/*if (NetworkingManager::IsCharacterInitialised(socketID))
	{
		SendFunctionCall("SyncNetworkPlayerPositions", socketID);
	}*/
	_connections[socketID]->getConnectionSocket()->send(sendPacket);
}

void NetworkingManager::SendEnemySpawnInfoResult(EnemyInfo* enemiesInfo, int length, int socketID)
{
	EnemySpawnInfoResult result;
	result.length = length;
	result.enemiesInfo = enemiesInfo;
	sf::Packet sendEnemyInfoPacket;
	sendEnemyInfoPacket << "SpawnNetworkedEnemies";
	sendEnemyInfoPacket << result;
	//SendFunctionCall("SpawnNetworkedEnemies", socketID);
	_connections[socketID]->getConnectionSocket()->send(sendEnemyInfoPacket);
}

void NetworkingManager::SendFunctionCall(std::string funcCallName, int socketID)
{
	sf::Packet packet;
	packet << funcCallName;
	if (socketID == -1)
	{
		std::cout << "Sending eventCall to server: " << funcCallName << std::endl;
		_mySocket->send(packet);
	}
	else
	{
		std::cout << "Sending eventCall to client: " << funcCallName << std::endl;
		_connections[socketID]->getConnectionSocket()->send(packet);
	}
}
// TODO make this one thing or something like that
void NetworkingManager::SendUpdatedNetworkData(std::vector<NetworkObject> networkObjects, Player* localPlayer)
{
	NetworkObjectUpdateData updateData;
	int updateArrayLength = networkObjects.size() + 1;
	updateData.playerLength = updateArrayLength;
	NetworkObjectPositionSyncVar* syncVars = new NetworkObjectPositionSyncVar[updateArrayLength];
	for (int i = 0; i < updateData.playerLength - 1; i++)
	{
		NetworkObjectPositionSyncVar var;
		var.objectID = networkObjects[i].GetID();
		var.newPosition = networkObjects[i].getPosition();
		syncVars[i] = var;
	}
	// Add on local player position
	NetworkObjectPositionSyncVar posSyncVar;
	posSyncVar.objectID = NetworkingManager::GetMyConnectionIndex();
	posSyncVar.newPosition = localPlayer->getPosition();
	syncVars[updateArrayLength - 1] = posSyncVar;
	// Set Update Data to point to sync vars
	updateData.playerPosSyncVars = syncVars;
	updateData.enemyPosSyncVars = 0;
	updateData.enemyLength = 0;
	sf::Packet posSyncPacket;
	posSyncPacket << "SyncNetworkPosition";
	posSyncPacket << updateData;
	_mySocket->send(posSyncPacket);
}

void NetworkingManager::SendUpdatedNetworkData(std::string eventCall, NetworkObjectUpdateData data, int socketID)
{
	sf::Packet packet;
	sf::String eventName = "ServerUpdateEnemyPositions";
	if (socketID == -1)
	{
		//SendFunctionCall(eventCall);
		
		packet << "ServerUpdateEnemyPositions";
		packet << data;
		_mySocket->send(packet);
	}
	else
	{
		//SendFunctionCall(eventCall, socketID);
		packet << "ServerUpdateEnemyPositions";
		packet << data;
		_connections[socketID]->getConnectionSocket()->send(packet);
	}
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

std::map<int, Player*> NetworkingManager::GetNetworkPlayers()
{
	return _players;
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
	packet << data.playerLength;
	for (int i = 0; i < data.playerLength; i++)
	{
		packet << data.playerPosSyncVars[i];
	}
	packet << data.enemyLength;
	for (int i = 0; i < data.enemyLength; i++)
	{
		packet << data.enemyPosSyncVars[i];
	}
	return packet;
}
sf::Packet& operator >> (sf::Packet& packet, NetworkObjectUpdateData& data)
{
	packet >> data.playerLength;
	data.playerPosSyncVars = new NetworkObjectPositionSyncVar[data.playerLength];
	for (int i = 0; i < data.playerLength; i++)
	{
		NetworkObjectPositionSyncVar resultPos;
		packet >> resultPos;
		data.playerPosSyncVars[i] = resultPos;
	}
	packet >> data.enemyLength;
	data.enemyPosSyncVars = new NetworkObjectPositionSyncVar[data.enemyLength];
	for (int i = 0; i < data.enemyLength; i++)
	{
		NetworkObjectPositionSyncVar resultPos;
		packet >> resultPos;
		data.enemyPosSyncVars[i] = resultPos;
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