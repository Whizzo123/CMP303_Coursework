#include "Chest.h"


Chest::Chest() {}

Chest::Chest(std::string textureName, sf::Vector2f pos, sf::Vector2f size, sf::RenderWindow* window, Input* input)
{
	//Load chest texture from path
	texture.loadFromFile("gfx/" + textureName+ ".png");
	//Initialize gameobject variables
	setTexture(&texture);
	setPosition(pos);
	setSize(size);
	setCollisionBox(sf::FloatRect(0, 0, size.x, size.y));
}