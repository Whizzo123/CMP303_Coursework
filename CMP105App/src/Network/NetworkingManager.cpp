#include "NetworkingManager.h"

// Static Initialisers
bool NetworkingManager::_server = false;
bool NetworkingManager::_serverStarted = false;
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
	_selector = new MSocketSelector();
	_listener->listen();
	std::cout << "Listening" << std::endl;
	MSocket* clientSocket = _listener->accept();
	_selector->add(clientSocket);
	_connections[_connectionIndex] = new NetworkConnection(_connectionIndex, clientSocket);
	_connectionIndex++;
	std::cout << "Client connected" << std::endl;
	sf::Packet sendPacket;
	sendPacket << sf::String("Hello this is the server!!!");
	clientSocket->send(sendPacket);
	_serverStarted = true;
	level.setNextLevel(2);
}

sf::Vector2f* NetworkingManager::GetPlayerPos(int& length)
{
	length = 5;
	return new sf::Vector2f[3];
}
void NetworkingManager::SearchForCalls()
{
	// Use selector to see if any sockets have sent data to be used

	int socketID = _selector->GrabReadySocket(_connections, _connectionIndex);
	if (socketID != -1)
	{
		sf::Packet recievePacket;
		_connections[socketID]->getConnectionSocket()->recieve(recievePacket);
		//TODO we need to send 2 packets one for the method name and another for the parameters
		FunctionCall<PlayerPosParams> call;
		std::string functionName;
		recievePacket >> call;
		if (functionName == "GetPlayerPos")
		{
			PlayerPosParams params;
			//params.positions = GetPlayerPos(params.length);
			GetPlayerPos(params.length);
			sf::Packet sendPacket;
			sendPacket << params;
			_connections[socketID]->getConnectionSocket()->send(sendPacket);
		}
	}
}

/// <summary>
/// Starts a client on this computer and joins with server
/// </summary>
/// <returns></returns>
bool NetworkingManager::StartClient(Level& level)
{
	_server = false;
	sf::IpAddress ip = "127.0.0.1";
	_mySocket = new MSocket(ip, port, false);
	sf::Packet recievePacket;
	_mySocket->recieve(recievePacket);
	sf::String recievedStringData;
	recievePacket >> recievedStringData;
	std::cout << "Recieved data" << recievedStringData.toAnsiString() << std::endl;
	// Load level
	level.setNextLevel(2);
	// Spawn players
	sf::Packet packet;
	packet << FunctionCall<PlayerPosParams>(sf::String("GetPlayerPos"), PlayerPosParams());
	CallServer(packet);
	PlayerPosParams params; 
	packet >> params;
	std::cout << "Number of players: " << params.length << std::endl;
}

void NetworkingManager::CallServer(sf::Packet packet)
{
	// Call this function on server-side
	_mySocket->send(packet);
	// Await function results
	_mySocket->recieve(packet);
}

bool NetworkingManager::JoinServer()
{

}

bool NetworkingManager::isServer()
{
	return _server;
}

bool NetworkingManager::isServerStarted()
{
	return _serverStarted;
}