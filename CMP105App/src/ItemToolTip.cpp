#include "ItemToolTip.h"

ItemToolTip::ItemToolTip(std::string itemName, int stackSize)
{
	//Load texture from path
	texture.loadFromFile("gfx/ToolTip.png");
	//Initialize gameobject variables
	setTexture(&texture);
	setSize(sf::Vector2f(300, 48));
	//Initialize variables
	textFont.loadFromFile("font/arial.ttf");
	textSize = 14.0f;
	padding = 20.0f;
	nameText.setFont(textFont);
	nameText.setCharacterSize(textSize);
	stackText.setFont(textFont);
	stackText.setCharacterSize(textSize);
	resetItem(itemName, stackSize);
	showing = false;
}

void ItemToolTip::addToChildObjects(std::string parameterName, std::string parameterValue)
{
	//Create text variables
	sf::Text* text = new sf::Text();
	text->setFont(textFont);
	text->setString(parameterName + parameterValue);
	text->setCharacterSize(14.0f);
	//Add to childobjects vector
	childObjects.push_back(text);
}

void ItemToolTip::clearChildren()
{
	childObjects.clear();
}

void ItemToolTip::draw(sf::RenderWindow* window)
{
	//Sets position of name text in tooltip
	nameText.setPosition(getPosition() + sf::Vector2f(10.0f, getSize().y / 4));
	window->draw(nameText);
	//Sets position of stack text in tooltip
	stackText.setPosition(getPosition() + sf::Vector2f(nameText.getLocalBounds().width + padding, getSize().y / 4));
	window->draw(stackText);
	//Sets position for rest of children
	for (int i = 0; i < childObjects.size(); i++)
	{
		childObjects[i]->setPosition(stackText.getPosition() + sf::Vector2f(stackText.getLocalBounds().width + 10.0f, 0) 
			+ sf::Vector2f(i * (childObjects[i]->getLocalBounds().width + 10.0f), 0));
		window->draw(*childObjects[i]);
	}
}

void ItemToolTip::resetItem(std::string itemName, int stackSize)
{
	//Resets name and stacksize for tooltip
	nameText.setString(itemName);
	stackText.setString("Count: " + std::to_string(stackSize));
}

void ItemToolTip::toggle()
{
	showing = !showing;
}

bool ItemToolTip::isShowing()
{
	return showing;
}

