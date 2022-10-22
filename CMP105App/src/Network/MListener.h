#pragma once

#include "SFML/Network.hpp"
#include "MSocket.h"

class MListener
{

public:
	MListener();
	MListener(unsigned short portNumber);
	~MListener();
	bool listen();
	MSocket* accept();

private:
	sf::TcpListener listener;
	unsigned short port;
};

