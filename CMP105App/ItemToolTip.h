#pragma once
#include "Framework/GameObject.h"
#include <iostream>

class ItemToolTip : public GameObject
{
public:
	ItemToolTip() {};
	ItemToolTip(std::string itemName, int stackSize);
	~ItemToolTip() {};
	void addToChildObjects(std::string parameterName, std::string parameterValue);
	void clearChildren();
	void resetItem(std::string itemName, int stackSize);
	void draw(sf::RenderWindow* window);
	void toggle();
	bool isShowing();

private:
	std::vector<sf::Text*> childObjects;
	sf::Font textFont;
	sf::Text nameText;
	sf::Text stackText;
	sf::Texture texture;
	float textSize;
	float padding;
	bool showing;
};

