#pragma once

#include "MListener.h"

static class NetworkingManager
{

public:
	static bool Find();
	static bool StartServer();
	static bool StartClient();
	static bool JoinServer();
	static bool isServer();

	static const unsigned short port = 4444;

private:
	static bool server;

};

