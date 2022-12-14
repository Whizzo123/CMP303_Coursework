#include "NetworkingManager.h"

// Static Initialisers
bool NetworkingManager::_server = false;
bool NetworkingManager::_serverStarted = false;
bool NetworkingManager::_clientStarted = false;
NetworkConnection** NetworkingManager::_connections = new NetworkConnection*[numberOfConnectionsAllowed];
//std::map<int, Player*> NetworkingManager::_players = std::map<int, Player*>();
int NetworkingManager::_connectionIndex = 1;
MListener* NetworkingManager::_listener = 0;
MSocket* NetworkingManager::_mySocket = 0;
MSocketSelector* NetworkingManager::_selector = 0;
int NetworkingManager::_myConnectionIndex = 0;
//Player* NetworkingManager::localPlayer = nullptr;
std::map<int, bool> NetworkingManager::changeStateOfNetworkObjects = std::map<int, bool>();
std::vector<NetworkObject*> NetworkingManager::networkObjects = std::vector<NetworkObject*>();
std::map<int, NetworkObject*> NetworkingManager::_playerNetworkObjects = std::map<int, NetworkObject*>();
std::map<int, bool> NetworkingManager::socketIDToCharacterInitialised = std::map<int, bool>();
float NetworkingManager::networkTime = 0.0f;
bool NetworkingManager::startTime = false;

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
	MSocket* clientSocket = _listener->accept();
	// Add client to NetworkConnections
	_connections[_connectionIndex] = new NetworkConnection(_connectionIndex, clientSocket);
	_selector->add(_connections[_connectionIndex]->getConnectionSocket());
	socketIDToCharacterInitialised[_connectionIndex] = false;
	// Send the client a welcome connection
	sf::Packet sendPacket;
	sendPacket << sf::String("Hello this is the server!!!");
	_connections[_connectionIndex]->getConnectionSocket()->send(sendPacket);
	// Send the client connection index to it
	auto orgTime = system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(orgTime);
	sf::Int32 time = sf::Int32(end_time);
	sf::Packet connectionIndexPacket;
	connectionIndexPacket << sf::Int32(_connectionIndex) << time;
	_connections[_connectionIndex]->getConnectionSocket()->send(connectionIndexPacket);
	StartTime(0.0f);
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
	std::cout << "Server message: " << recievedStringData.toAnsiString() << std::endl;
	sf::Packet connectionIndexRecvPacket;
	_mySocket->recieve(connectionIndexRecvPacket);
	connectionIndexRecvPacket >> _myConnectionIndex;
	std::cout << "My connection Index: " << _myConnectionIndex << std::endl;
	sf::Int32 time;
	connectionIndexRecvPacket >> time;
	StartTime(std::chrono::system_clock::to_time_t(system_clock::now()) - time);
	_clientStarted = true;
	return true;
}

int NetworkingManager::FindReadySockets()
{
	_selector->wait();
	int socketID = _selector->GrabReadySocket(_connections, _connectionIndex);
	return socketID;
}

sf::Packet* NetworkingManager::RecievePacketOnSocket(int socketID)
{
	sf::Packet* recievePacket = new sf::Packet();
	bool okay = _connections[socketID]->getConnectionSocket()->recieve(*recievePacket);
	if (okay)
		return recievePacket;
	else
		return nullptr;
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
void NetworkingManager::SendUpdatedNetworkData(std::vector<NetworkObject> networkObjects)
{
	NetworkObjectUpdateData updateData;
	int updateArrayLength = networkObjects.size();
	updateData.playerLength = updateArrayLength;
	NetworkObjectPositionSyncVar* syncVars = new NetworkObjectPositionSyncVar[updateArrayLength];
	for (int i = 0; i < updateData.playerLength; i++)
	{
		NetworkObjectPositionSyncVar var;
		var.objectID = networkObjects[i].GetID();
		var.newPosition = networkObjects[i].getPosition();
		syncVars[i] = var;
	}
	// Set Update Data to point to sync vars
	updateData.playerPosSyncVars = syncVars;
	sf::Packet posSyncPacket;
	updateData.enemyNetworkObjects = 0;
	updateData.enemyLength = 0;
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
		packet << "ServerUpdateEnemyPositions";
		packet << data;
		_mySocket->send(packet);
	}
	else
	{
		packet << "ServerUpdateEnemyPositions";
		packet << data;
		_connections[socketID]->getConnectionSocket()->send(packet);
	}
}

void NetworkingManager::SendPlayerAttackData(PlayerAttackData data)
{
	sf::Packet packet;
	if (isServer())
	{
		packet << "PlayerAttackedEvent";
		packet << data;
		for (int i = 1; i < GetNumConnections(); i++)
		{
			if (i != data.playerID)
				_connections[i]->getConnectionSocket()->send(packet);
		}
	}
	else
	{
		packet << "SyncPlayerAttackedEvent";
		packet << data;
		_mySocket->send(packet);
	}
}

void NetworkingManager::SendPlayerReviveData(int playerID, sf::Vector2f pos)
{
	sf::Packet packet;
	if (isServer())
	{
		packet << "PlayerReviveEvent";
		packet << sf::Int32(playerID);
		packet << pos;
		for (int i = 1; i < GetNumConnections(); i++)
		{
			if (i != playerID)
				_connections[i]->getConnectionSocket()->send(packet);
		}
	}
	else
	{
		packet << "SyncPlayerReviveEvent";
		packet << sf::Int32(playerID);
		packet << pos;
		_mySocket->send(packet);
	}
}

void NetworkingManager::SendInventorySyncData(InventorySyncData data)
{
	sf::Packet packet;
	std::cout << "Entering NetworkingManager::SendInventorySyncData" << std::endl;
	if (isServer())
	{
		
		packet << "InventoryChangeEvent";
		packet << data;
		for (int i = 1; i < GetNumConnections(); i++)
		{
			if (i != data.playerID)
			{
				std::cout << "Sending inventory sync data from server" << std::endl;
				_connections[i]->getConnectionSocket()->send(packet);
			}
		}
	}
	else
	{
		std::cout << "Sending inventory sync data from client to server" << std::endl;
		packet << "SyncInventoryChangeEvent";
		packet << data;
		_mySocket->send(packet);
	}
}

void NetworkingManager::SendChestSpawnInfoResult(ChestSpawnInfoResult result, int socketID)
{
	sf::Packet sendChestInfoPacket;
	sendChestInfoPacket << "SpawnNetworkedChests";
	sendChestInfoPacket << result;
	_connections[socketID]->getConnectionSocket()->send(sendChestInfoPacket);
}

void NetworkingManager::SendNextLevelMessage()
{
	sf::Packet sendNextLevelPacket;
	sendNextLevelPacket << "NextLevel";
	if (isServer())
	{
		for (int i = 0; i < GetNumConnections(); i++)
		{
			if (i != NetworkingManager::GetMyConnectionIndex())
				_connections[i]->getConnectionSocket()->send(sendNextLevelPacket);
		}
	}
	else
		_mySocket->send(sendNextLevelPacket);
}

void NetworkingManager::StartTime(float offsetTime) 
{
	startTime = true; 
	networkTime += offsetTime; 
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
		packet << data.enemyNetworkObjects[i];
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
	data.enemyNetworkObjects = new EnemyNetworkObject[data.enemyLength];
	for (int i = 0; i < data.enemyLength; i++)
	{
		EnemyNetworkObject enemy;
		packet >> enemy;
		data.enemyNetworkObjects[i] = enemy;
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

sf::Packet& operator << (sf::Packet& packet, const NetworkObjectTargetSyncVar data)
{
	return packet << data.objectID << data.targetObjectID;
}

sf::Packet& operator >> (sf::Packet& packet, NetworkObjectTargetSyncVar& data)
{
	return packet >> data.objectID >> data.targetObjectID;
}

sf::Packet& operator << (sf::Packet& packet, const EnemyNetworkObject data)
{
	return packet << data.objectID << data.position << data.velocity << data.targetedPlayerID;
}

sf::Packet& operator >> (sf::Packet& packet, EnemyNetworkObject& data)
{
	return packet >> data.objectID >> data.position >> data.velocity >> data.targetedPlayerID;
}

sf::Packet& operator << (sf::Packet& packet, const PlayerAttackData data)
{
	return packet << data.playerID << data.enemyID;
}
sf::Packet& operator >> (sf::Packet& packet, PlayerAttackData& data)
{
	return packet >> data.playerID >> data.enemyID;
}

sf::Packet& operator << (sf::Packet& packet, const InventorySyncData data)
{
	return packet << data.invID << data.slotID << data.playerID << data.otherInvID << data.otherInvSlotID << data.isAdding;
}

sf::Packet& operator >> (sf::Packet& packet, InventorySyncData& data)
{
	return packet >> data.invID >> data.slotID >> data.playerID >> data.otherInvID >> data.otherInvSlotID >> data.isAdding;
}

sf::Packet& operator << (sf::Packet& packet, const ItemData data)
{
	return packet << data.itemIndex << data.itemType;
}
sf::Packet& operator >> (sf::Packet& packet, ItemData& data)
{
	return packet >> data.itemIndex >> data.itemType;
}
sf::Packet& operator << (sf::Packet& packet, const ChestSpawnInfo data)
{
	packet << data.position << data.itemsCount;
	for (int i = 0; i < data.itemsCount; i++)
	{
		packet << data.items[i];
	}
	return packet;
}
sf::Packet& operator >> (sf::Packet& packet, ChestSpawnInfo& data)
{
	packet >> data.position >> data.itemsCount;
	data.items = new ItemData[data.itemsCount];
	for (int i = 0; i < data.itemsCount; i++)
	{
		ItemData item;
		packet >> item;
		data.items[i] = item;
	}
	return packet;
}
sf::Packet& operator << (sf::Packet& packet, const ChestSpawnInfoResult data)
{
	packet << data.length;
	for (int i = 0; i < data.length; i++)
	{
		packet << data.chestsInfo[i];
	}
	return packet;
}
sf::Packet& operator >> (sf::Packet& packet, ChestSpawnInfoResult& data)
{
	packet >> data.length;
	data.chestsInfo = new ChestSpawnInfo[data.length];
	for (int i = 0; i < data.length; i++)
	{
		ChestSpawnInfo info;
		packet >> info;
		data.chestsInfo[i] = info;
	}
	return packet;
}

sf::Packet& operator << (sf::Packet& packet, const milliseconds data)
{
	return packet << data;
}
sf::Packet& operator >> (sf::Packet* packet, milliseconds& data)
{
	return packet >> data;
}