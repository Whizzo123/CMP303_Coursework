#include "NetworkButton.h"

NetworkButton::NetworkButton()
{

}

NetworkButton::NetworkButton(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window)
	: Button(textureName, pos, size, input, window)
{
	
}

void NetworkButton::onClick()
{
	clicked = true;
}