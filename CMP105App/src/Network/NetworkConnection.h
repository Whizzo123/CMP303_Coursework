#pragma once

#include "MSocket.h"

class NetworkConnection
{

public:
	NetworkConnection() : _connectionID{ -1 }, _connectionSocket{ 0 } {};
	NetworkConnection(int id, MSocket* socket);
	~NetworkConnection();
	int getConnectionID();
	MSocket* getConnectionSocket();

private:
	int _connectionID;
	MSocket* _connectionSocket;

};

