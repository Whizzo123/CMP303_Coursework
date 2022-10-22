#include "LevelLoaderButton.h"

LevelLoaderButton::LevelLoaderButton(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window, int levelIndex)
	: Button(textureName, pos, size, input, window)
{
	this->levelIndex = levelIndex;
}

int LevelLoaderButton::getLevelIndex()
{
	return levelIndex;
}

void LevelLoaderButton::onClick()
{
	clicked = true;
}