#pragma once
#include "Button.h"
class LevelLoaderButton : public Button
{
public:
	LevelLoaderButton() {}
	LevelLoaderButton(std::string textureName, sf::Vector2f pos, sf::Vector2f size, Input* input, sf::RenderWindow* window, int levelIndex);
	~LevelLoaderButton() {}
	void onClick() override;
	int getLevelIndex();

private:
	int levelIndex;
};

