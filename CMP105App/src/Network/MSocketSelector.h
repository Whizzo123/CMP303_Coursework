#pragma once

#include "NetworkConnection.h"

class MSocketSelector
{

public:
	MSocketSelector();
	~MSocketSelector();

	void add(MSocket* socket);
	bool wait(float seconds = 0.01f);
	int GrabReadySocket(NetworkConnection** connectons, int numberOfConnections);

private:
	sf::SocketSelector selector;
	bool isReady(MSocket* socket);
};

