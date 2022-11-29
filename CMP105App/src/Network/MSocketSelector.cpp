#include "MSocketSelector.h"

MSocketSelector::MSocketSelector()
{

}

MSocketSelector::~MSocketSelector()
{
}


/// <summary>
/// Wrapper for the SFML socket selector function add 
/// </summary>
/// <param name="socket"></param>
void MSocketSelector::add(MSocket* socket)
{
	selector.add(*socket->getSFMLSocket());
}

/// <summary>
/// Wait for some amount of time looking for send messages from sockets
/// </summary>
/// <param name="seconds"></param>
/// <returns></returns>
bool MSocketSelector::wait(float seconds)
{
	return selector.wait(sf::seconds(seconds));
}

/// <summary>
/// Searches through list of network connections to find socket that is ready
/// </summary>
/// <param name="connections"></param>
/// <param name="numberOfConnections"></param>
/// <returns></returns>
int MSocketSelector::GrabReadySocket(NetworkConnection** connections, int numberOfConnections)
{
	// Starts at 1 as the server is index of 0
	for (int i = 1; i < numberOfConnections; i++)
	{
		if (isReady(connections[i]->getConnectionSocket()))
		{
			return i;
		}
	}
	return -1;
}

bool MSocketSelector::isReady(MSocket* socket)
{
	sf::TcpSocket* sfSocket = socket->getSFMLSocket();
	return selector.isReady(*sfSocket);
}
