#include "MSocket.h"


MSocket::MSocket() : socketMsgBuffer(new char[MESSAGESIZE])
{

}

MSocket::MSocket(sf::IpAddress ip, unsigned short port, bool isListener, bool blocking)
{
	if (!isListener)
	{
		// Create sfml client socket
		sf::Socket::Status status = socket.connect(ip, port);
		// Check if socket created without error
		if (status != sf::Socket::Done)
		{
			std::cout << "ERROR: Socket creation error for ip: " << ip.toString() << " and port: " << port << std::endl;
		}
	}
	socket.setBlocking(blocking);
	// Initialise socket message buffer
	socketMsgBuffer = new char[MESSAGESIZE];
	memset(socketMsgBuffer, '-', MESSAGESIZE);
}

MSocket::~MSocket()
{
	socket.disconnect();
}

void MSocket::setBlocking(bool isBlocking)
{
	socket.setBlocking(isBlocking);
}

/// <summary>
/// Wrapper method for SFML socket recieve function
/// </summary>
/// <param name="msgBuffer"></param>
/// <param name="msgSize"></param>
/// <param name="count"></param>
/// <returns></returns>
bool MSocket::recieve()
{
	size_t count = 0;
	// Check we recieved the message okay
	if (socket.receive(socketMsgBuffer, MESSAGESIZE, count))
	{
		std::cout << "ERROR: Socket recieve error" << std::endl;
		return false;
	}
	return true;
}


bool MSocket::recieve(sf::Packet& packet)
{
	sf::Socket::Status status = socket.receive(packet);
	if (status != sf::Socket::Status::Done)
	{
		//std::cout << "ERROR: Socket packet recieve error" << std::endl;
		return false;
	}
	return true;
}

/// <summary>
/// Wrapper for SFML TcpSocket send method
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
bool MSocket::send(std::string data)
{
	// Copy data to message buffer
	memset(socketMsgBuffer, '-', MESSAGESIZE);
	memcpy(socketMsgBuffer, data.c_str(), std::min((int)data.size(), MESSAGESIZE));
	 sf::Socket::Status status = socket.send(socketMsgBuffer, MESSAGESIZE);
	 // Check we send the message okay
	if (status != sf::Socket::Status::Done)
	{
		std::cout << "ERROR: Socket send error" << status << std::endl;
		return false;
	}

	return true;
}

bool MSocket::send(sf::Packet packet)
{
	sf::Socket::Status status = socket.send(packet);
	if (status != sf::Socket::Status::Done)
	{
		std::cout << "ERROR: Socket send error" << status << std::endl;
		return false;
	}
	return true;
}

sf::TcpSocket* MSocket::getSFMLSocket()
{
	return &socket;
}

char* MSocket::getMessage()
{
	return socketMsgBuffer;
}