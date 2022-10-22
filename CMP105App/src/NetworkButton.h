#pragma once

#include "Button.h"
#include "Network/NetworkingManager.h"

class NetworkButton : public Button
{
public:
	NetworkButton();
	NetworkButton(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window, bool toStartServer, Level* level);
	~NetworkButton() {}
	void onClick() override;

private:
	bool startServer;
	Level* _level;

};

