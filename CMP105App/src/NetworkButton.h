#pragma once

#include "Button.h"


class NetworkButton : public Button
{
public:
	NetworkButton();
	NetworkButton(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window);
	~NetworkButton() {}
	void onClick() override;
	bool isClicked() { return clicked; }
private:
	bool clicked;

};

