#pragma once
#include "Framework/GameObject.h"
#include "Framework/Collision.h"
#include <iostream>
class Button : public GameObject
{
public:
	Button() {}
	Button(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window);
	void handleInput(float dt) override;
	void update(float dt) override;
	virtual void onClick();
	bool isClicked();
	void reset();
protected:
	//For whether mouse is over button
	bool mousedOver;
	//Has button been clicked on
	bool clicked;
	//Button texture
	sf::Texture texture;
};

