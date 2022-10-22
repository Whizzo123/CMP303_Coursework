#include "NetworkingManager.h"

// Static Initialisers
bool NetworkingManager::_server = false;
NetworkConnection** NetworkingManager::_connections = new NetworkConnection*[numberOfConnectionsAllowed];
int NetworkingManager::_connectionIndex = 0;
MListener* NetworkingManager::_listener = 0;

bool NetworkingManager::Find()
{

}

/// <summary>
/// Starts a server on this computer
/// </summary>
/// <returns></returns>
bool NetworkingManager::StartServer(Level& level)
{
	_server = true;
	_listener = new MListener(port);
	_listener->listen();
	std::cout << "Listening" << std::endl;
	MSocket* clientSocket = _listener->accept();
	// Currently really doesn't like doing this NetworkConnection array
	_connections[_connectionIndex] = new NetworkConnection(_connectionIndex, clientSocket);
	std::cout << "Client connected" << std::endl;
	clientSocket->send("Hello this is the server!!");
	while (true)
	{

	}
	//level.setNextLevel(2);
}


/// <summary>
/// Starts a client on this computer and joins with server
/// </summary>
/// <returns></returns>
bool NetworkingManager::StartClient(Level& level)
{
	_server = false;
	sf::IpAddress ip = "127.0.0.1";
	MSocket* socket = new MSocket(ip, port, false);
	socket->recieve();
	std::cout << "Recieved data" << socket->getMessage() << std::endl;
	
	// Load level
	//level.setNextLevel(2);
}

bool NetworkingManager::JoinServer()
{

}

bool NetworkingManager::isServer()
{
	return _server;
}