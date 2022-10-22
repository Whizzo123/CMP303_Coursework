#include "NetworkingManager.h"

bool NetworkingManager::server = false;

bool NetworkingManager::Find()
{

}

bool NetworkingManager::StartServer()
{
	server = true;
	MListener* listener = new MListener(port);
	listener->listen();
	std::cout << "Listening" << std::endl;
	MSocket* clientSocket = listener->accept();
	std::cout << "Client connected" << std::endl;
	clientSocket->send("Hello this is the server!!");
	while (true)
	{

	}
}

bool NetworkingManager::StartClient()
{
	server = false;
	sf::IpAddress ip = "127.0.0.1";
	MSocket* socket = new MSocket(ip, port, false);
	socket->recieve();
	std::cout << "Recieved data" << std::endl;
}

bool NetworkingManager::JoinServer()
{

}

bool NetworkingManager::isServer()
{
	return server;
}