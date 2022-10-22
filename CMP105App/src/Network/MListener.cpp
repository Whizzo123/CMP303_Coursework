#include "MListener.h"

MListener::MListener() : port(4444)
{

}

MListener::MListener(unsigned short portNumber)
	: port{portNumber}
{

}

MListener::~MListener()
{

}


/// <summary>
/// Wrapper for SFML TcpListener function listen
/// </summary>
/// <returns></returns>
bool MListener::listen()
{
	// Check if any errors on listening on socket
	if (listener.listen(port) != sf::Socket::Done)
	{
		std::cout << "ERROR: Listener error on listen" << std::endl;
		return false;
	}
	return true;
}


/// <summary>
/// Wrapper for SFML TcpListener function accept
/// </summary>
/// <returns></returns>
MSocket* MListener::accept()
{
	MSocket* mSocket = new MSocket();
	if (listener.accept(*mSocket->getSFMLSocket()) != sf::Socket::Done) 
	{
		std::cout << "ERROR: Listener error on accept" << std::endl;
		return nullptr;
	}
	return mSocket;
}

