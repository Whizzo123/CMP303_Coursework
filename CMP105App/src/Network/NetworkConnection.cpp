#include "NetworkConnection.h"

NetworkConnection::NetworkConnection(int id, MSocket* socket)
	: _connectionID{id}, _connectionSocket{socket}
{

}

NetworkConnection::~NetworkConnection()
{
	if (_connectionSocket)
	{
		delete(_connectionSocket);
		_connectionSocket = 0;
	}
}

int NetworkConnection::getConnectionID()
{
	return _connectionID;
}

MSocket* NetworkConnection::getConnectionSocket()
{
	return _connectionSocket;
}