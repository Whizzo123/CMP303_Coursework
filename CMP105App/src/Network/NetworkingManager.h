#pragma once

#include "MListener.h"
#include "NetworkConnection.h"
#include "MSocketSelector.h"
#include "../Level.h"

static class NetworkingManager
{

public:
	static bool Find();
	static bool StartServer(Level& level);
	static bool StartClient(Level& level);
	static bool JoinServer();
	static bool isServer();
	static bool isServerStarted();
	static void CallServer(sf::Packet packet);
	static sf::Vector2f* GetPlayerPos(int& length);
	static void SearchForCalls();

	static const unsigned short port = 4444;
	static const int numberOfConnectionsAllowed = 2;
	

private:
	// Server Variables
	static bool _server;
	static bool _serverStarted;
	static NetworkConnection** _connections; 
	static int _connectionIndex;
	static MListener* _listener;
	static MSocketSelector* _selector;
	// Client Variables
	static MSocket* _mySocket;
	

};

template <class T>
struct FunctionCall
{
	sf::String functionName;
	T data;

	FunctionCall() {}
	FunctionCall(sf::String name, T parameters)
	{
		functionName = name;
		data = parameters;
	}
	
};

template <class T>
sf::Packet& operator << (sf::Packet& packet, const FunctionCall<T>& functioncall)
{
	return packet << functioncall.functionName << functioncall.data;
}

template <class T>
sf::Packet& operator >> (sf::Packet& packet, FunctionCall<T>& functioncall)
{
	return packet >> functioncall.functionName >> functioncall.data;
}

struct PlayerPosParams
{
	int length;
	//sf::Vector2f* positions;
};

sf::Packet& operator << (sf::Packet& packet, const PlayerPosParams& params)
{
	return packet << params.length;
}

sf::Packet& operator >> (sf::Packet& packet, PlayerPosParams& params)
{
	return packet >> params.length;
}

/*sf::Packet& operator >> (sf::Packet& packet, sf::Vector2f* position)
{
	return packet >> position;
}*/

