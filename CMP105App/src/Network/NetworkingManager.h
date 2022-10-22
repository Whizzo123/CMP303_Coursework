#pragma once

#include "MListener.h"
#include "NetworkConnection.h"
#include "../Level.h"

static class NetworkingManager
{

public:
	static bool Find();
	static bool StartServer(Level& level);
	static bool StartClient(Level& level);
	static bool JoinServer();
	static bool isServer();

	static const unsigned short port = 4444;
	static const int numberOfConnectionsAllowed = 2;
	

private:
	static bool _server;
	static NetworkConnection** _connections; 
	static int _connectionIndex;
	static MListener* _listener;
};

