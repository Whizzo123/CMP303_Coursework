#pragma once

#include "NetworkConnection.h"

class MSocketSelector
{

public:
	MSocketSelector();
	~MSocketSelector();

	void add(MSocket* socket);
	bool wait(float seconds);
	int GrabReadySocket(NetworkConnection** connectons, int numberOfConnections);

private:
	sf::SocketSelector selector;
	bool isReady(MSocket* socket);
};

