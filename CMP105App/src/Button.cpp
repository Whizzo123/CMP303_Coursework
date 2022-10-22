#include "Button.h"

Button::Button(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window)
{
	//Load texture for button
	texture.loadFromFile("gfx/" + textureName + ".png");
	setTexture(&texture);
	//Initialize gameobject variables
	setPosition(pos);
	setSize(size);
	setInput(input);
	setWindow(window);
	setCollisionBox(sf::FloatRect(0 , 0, getSize().x, getSize().y));
	//Initialize button variables
	mousedOver = false;
	clicked = false;
}

void Button::handleInput(float dt)
{
	//If player is mousing over button
	if (mousedOver)
	{
		//Check for a click
		if(input->isLeftMousePressed())
			onClick();
	}
}

void Button::update(float dt)
{
	//Shift mouse position to compensate for view movement
	sf::Vector2i mousePos = sf::Vector2i(input->getMouseX() + window->getView().getCenter().x - window->getSize().x / 2,
		input->getMouseY() + window->getView().getCenter().y - window->getSize().y / 2);
	//If button and mouse are colliding
	if (Collision::checkBoundingBox(this, mousePos))
	{
		//We are mousing over
		mousedOver = true;
	}
	else
	{
		//Not mousing over
		mousedOver = false;
	}
}


void Button::onClick()
{
	clicked = true;
}

void Button::reset()
{
	clicked = false;
}

bool Button::isClicked()
{
	return clicked;
}