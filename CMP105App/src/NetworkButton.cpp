#include "NetworkButton.h"

NetworkButton::NetworkButton() : startServer{true}
{

}

NetworkButton::NetworkButton(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window, bool toStartServer, Level* level)
	: _level{ level }, Button(textureName, pos, size, input, window)
{
	startServer = toStartServer;
}

void NetworkButton::onClick()
{
	clicked = true;
	if (startServer)
	{
		NetworkingManager::StartServer(*_level);
	}
	else
	{
		NetworkingManager::StartClient(*_level);
	}
}