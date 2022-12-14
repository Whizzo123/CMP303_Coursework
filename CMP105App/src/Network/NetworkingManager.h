#pragma once

#include "MListener.h"
#include "NetworkConnection.h"
#include "MSocketSelector.h"
#include <string>
#include "../NetworkObject.h"
#include "../Enemy.h"
#include <chrono>

using namespace std::chrono;

struct InventorySyncData
{
	sf::Int32 invID;
	sf::Int32 slotID;
	sf::Int32 playerID;
	sf::Int32 otherInvID;
	sf::Int32 otherInvSlotID;
	sf::Int32 isAdding;
};

struct ItemData
{
	sf::Int32 itemIndex;
	sf::Int32 itemType;
};

struct PlayerAttackData
{
	sf::Int32 playerID;
	sf::Int32 enemyID;
};

struct EnemySpawnInfoResult
{
	sf::Int32 length;
	EnemyInfo* enemiesInfo;
};

struct ChestSpawnInfo
{
	sf::Vector2f position;
	sf::Int32 itemsCount;
	ItemData* items;
};

struct ChestSpawnInfoResult
{
	sf::Int32 length;
	ChestSpawnInfo* chestsInfo;
};



struct NetworkObjectPositionSyncVar
{
	sf::Int32 objectID;
	sf::Vector2f newPosition;
};

struct NetworkObjectTargetSyncVar
{
	sf::Int32 objectID;
	sf::Int32 targetObjectID;
};

struct EnemyNetworkObject
{
	sf::Int32 objectID;
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Int32 targetedPlayerID;
};

struct NetworkObjectUpdateData
{
	sf::Int32 playerLength;
	sf::Int32 enemyLength;
	NetworkObjectPositionSyncVar* playerPosSyncVars;
	EnemyNetworkObject* enemyNetworkObjects;
};

class NetworkingManager
{
public:
	static bool Find();
	static bool StartServer(Input* input, sf::RenderWindow* window, AudioManager* audioManager);
	static bool StartClient(Input* input, sf::RenderWindow* window, AudioManager* audioManager);
	static bool JoinServer();
	static bool isServer();
	static bool isClientStarted();
	static bool isServerStarted();
	//static std::map<int, Player*> GetNetworkPlayers();
	static void AddNetworkObject(NetworkObject* object);
	static int FindReadySockets();
	static sf::Packet* RecievePacketOnSocket(int socketID);
	static sf::Packet* RecievePacketOnSocket();
	static void SendPlayerPosResultPacket(std::vector<sf::Vector2f> positions, int socketID);
	static void SendEnemySpawnInfoResult(EnemyInfo* enemiesInfo, int length, int socketID);
	static void SendFunctionCall(std::string funcCallName, int socketID = -1);
	static void SendUpdatedNetworkData(std::vector<NetworkObject> networkObjects);
	static void SendUpdatedNetworkData(std::string eventCall, NetworkObjectUpdateData data, int socketID = -1);
	static void SendPlayerAttackData(PlayerAttackData data);
	static void SendPlayerReviveData(int playerID, sf::Vector2f pos);
	static void SendInventorySyncData(InventorySyncData data);
	static void SendChestSpawnInfoResult(ChestSpawnInfoResult result, int socketID);
	static void SendNextLevelMessage();
	static int GetNumConnections() { return _connectionIndex; }
	static int GetMyConnectionIndex() { return _myConnectionIndex; }
	static std::map<int, bool> GetNObjectChangeState() { return changeStateOfNetworkObjects; }
	static std::vector<NetworkObject*> GetNetworkObjects() { return networkObjects; }
	static std::map<int, NetworkObject*> GetPlayerNetworkObjects() { return _playerNetworkObjects; }
	static void SetToBlock(bool value) { _mySocket->setBlocking(value); }
	static void SetCharacterInitialised(int socketID) { socketIDToCharacterInitialised[socketID] = true; }
	static bool IsCharacterInitialised(int socketID) { return socketIDToCharacterInitialised[socketID]; }
	static void AddPlayerNetworkObject(int index, NetworkObject* player) { _playerNetworkObjects[index] = player; }
	static NetworkObject* GetPlayerNetworkObject(int index) { return _playerNetworkObjects[index]; }
	static float GetCurrentTime() { return networkTime; }
	static void UpdateTime(float deltaTime) { if (startTime) { networkTime += deltaTime; } }
	static void StartTime(float offsetTime);



	static const unsigned short port = 4444;
	static const int numberOfConnectionsAllowed = 2;
	//static Player* localPlayer;
	

private:
	// Server Variables
	static bool _server;
	static bool _serverStarted;
	static NetworkConnection** _connections; 
	static std::map<int, NetworkObject*> _playerNetworkObjects;
	static int _connectionIndex;
	static MListener* _listener;
	static MSocketSelector* _selector;
	// Client Variables
	static MSocket* _mySocket;
	static int _myConnectionIndex;
	static bool _clientStarted;
	static std::map<int, bool> changeStateOfNetworkObjects;
	static std::vector<NetworkObject*> networkObjects;
	static std::map<int, bool> socketIDToCharacterInitialised;
	static float networkTime;
	static bool startTime;
};



struct PlayerPosResult
{
	sf::Int32 vecLength;
	std::vector<sf::Vector2f> resultPositions;
};





struct FunctionName
{
	sf::String funcName;
};
sf::Packet& operator << (sf::Packet& packet, const PlayerPosResult& results);
sf::Packet& operator >> (sf::Packet& packet, PlayerPosResult& results);
sf::Packet& operator << (sf::Packet& packet, const sf::Vector2f& position);
sf::Packet& operator >> (sf::Packet& packet, sf::Vector2f& position);
sf::Packet& operator << (sf::Packet& packet, const FunctionName& name);
sf::Packet& operator >> (sf::Packet& packet, FunctionName& name);
sf::Packet& operator << (sf::Packet& packet, const NetworkObjectPositionSyncVar& name);
sf::Packet& operator >> (sf::Packet& packet, NetworkObjectPositionSyncVar& name);
sf::Packet& operator << (sf::Packet& packet, const NetworkObjectUpdateData& name);
sf::Packet& operator >> (sf::Packet& packet, NetworkObjectUpdateData& name);
sf::Packet& operator << (sf::Packet& packet, const EnemySpawnInfoResult& data);
sf::Packet& operator >> (sf::Packet& packet, EnemySpawnInfoResult& data);
sf::Packet& operator << (sf::Packet& packet, const EnemyInfo& data);
sf::Packet& operator >> (sf::Packet& packet, EnemyInfo& data);
sf::Packet& operator << (sf::Packet& packet, const EnemyType& data);
sf::Packet& operator >> (sf::Packet& packet, EnemyType& data);
sf::Packet& operator << (sf::Packet& packet, const NetworkObjectTargetSyncVar data);
sf::Packet& operator >> (sf::Packet& packet, NetworkObjectTargetSyncVar& data);
sf::Packet& operator << (sf::Packet& packet, const EnemyNetworkObject data);
sf::Packet& operator >> (sf::Packet& packet, EnemyNetworkObject& data);
sf::Packet& operator << (sf::Packet& packet, const PlayerAttackData data);
sf::Packet& operator >> (sf::Packet& packet, PlayerAttackData& data);
sf::Packet& operator << (sf::Packet& packet, const InventorySyncData data);
sf::Packet& operator >> (sf::Packet& packet, InventorySyncData& data);
sf::Packet& operator << (sf::Packet& packet, const ItemData data);
sf::Packet& operator >> (sf::Packet& packet, ItemData& data);
sf::Packet& operator << (sf::Packet& packet, const ChestSpawnInfo data);
sf::Packet& operator >> (sf::Packet& packet, ChestSpawnInfo& data);
sf::Packet& operator << (sf::Packet& packet, const ChestSpawnInfoResult data);
sf::Packet& operator >> (sf::Packet& packet, ChestSpawnInfoResult& data);