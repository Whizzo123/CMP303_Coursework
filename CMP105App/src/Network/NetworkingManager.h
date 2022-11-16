#pragma once

#include "MListener.h"
#include "NetworkConnection.h"
#include "MSocketSelector.h"
#include <string>
#include "NetworkPlayer.h"
#include "../GameLevel.h"


struct EnemySpawnInfoResult
{
	sf::Int32 length;
	EnemyInfo* enemiesInfo;
};


static class NetworkingManager
{



public:
	static bool Find();
	static bool StartServer(Level& level, Input* input, sf::RenderWindow* window, AudioManager* audioManager);
	static bool StartClient(Level& level, Input* input, sf::RenderWindow* window, AudioManager* audioManager);
	static bool JoinServer();
	static bool isServer();
	static bool isClientStarted();
	static bool isServerStarted();
	static sf::Packet CallServer(sf::String funcName);
	static std::vector<sf::Vector2f> GetPlayerPos();
	static void SearchForCalls(float dt);
	static std::vector<NetworkPlayer*> GetNetworkPlayers();
	static void ClientNetworkSync(float dt);
	static void SyncNetworkPlayerPositions(std::vector<sf::Vector2f> positions);
	static void AddNetworkObject(NetworkObject* object);
	static sf::Packet PacketUpdatedNetworkObjectData();
	static std::vector<NetworkObject> GetUpdatedNetworkObjects();
	static void SyncNetworkPosition(int socketID);
	static void SpawnNetworkedEnemies();
	static void GetEnemyInfoForClient(int socketID);
	static void CreateLocalPlayer(Input* input, sf::RenderWindow* window, AudioManager* audio);

	static const unsigned short port = 4444;
	static const int numberOfConnectionsAllowed = 2;
	static Player* localPlayer;
	static GameLevel* currentLevel;
	

private:
	// Server Variables
	static bool _server;
	static bool _serverStarted;
	static NetworkConnection** _connections; 
	static std::map<int, NetworkPlayer*> _players;
	static int _connectionIndex;
	static MListener* _listener;
	static MSocketSelector* _selector;
	// Client Variables
	static MSocket* _mySocket;
	static int _myConnectionIndex;
	static bool _clientStarted;
	static std::map<int, bool> changeStateOfNetworkObjects;
	static std::vector<NetworkObject*> networkObjects;
	
};



struct PlayerPosResult
{
	sf::Int32 vecLength;
	std::vector<sf::Vector2f> resultPositions;
};

struct NetworkObjectPositionSyncVar
{
	sf::Int32 objectID;
	sf::Vector2f newPosition;
};

struct NetworkObjectUpdateData
{
	sf::Int32 length;
	NetworkObjectPositionSyncVar* posSyncVars;
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


