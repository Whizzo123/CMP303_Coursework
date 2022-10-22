#pragma once
#include <iostream>
#include <algorithm>
#include "SFML/Network.hpp"

class MSocket
{

public:
	MSocket();
	MSocket(sf::IpAddress ip, unsigned short port, bool isListener);
	~MSocket();
	bool recieve();
	bool send(std::string data);
	sf::TcpSocket* getSFMLSocket();
private:

	const int MESSAGESIZE = 256;

	sf::TcpSocket socket;
	char* socketMsgBuffer;
};

