#pragma once
#include <iostream>
#include <algorithm>
#include "SFML/Network.hpp"

class MSocket
{

public:
	MSocket();
	MSocket(sf::IpAddress ip, unsigned short port, bool isListener = true, bool blocking=true);
	~MSocket();
	bool recieve();
	bool recieve(sf::Packet& packet);
	bool send(std::string data);
	bool send(sf::Packet packet);
	sf::TcpSocket* getSFMLSocket();
	char* getMessage();
	void setBlocking(bool isBlocking);

private:

	const int MESSAGESIZE = 256;

	sf::TcpSocket socket;
	char* socketMsgBuffer;
};

