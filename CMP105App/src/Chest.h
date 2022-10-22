#pragma once
#include "Framework/GameObject.h"
class Chest : public GameObject
{
public:
	Chest();
	Chest(std::string textureName, sf::Vector2f pos, sf::Vector2f size, sf::RenderWindow* window, Input* input);
	~Chest() {}

private:
	//Chest texture
	sf::Texture texture;
};

