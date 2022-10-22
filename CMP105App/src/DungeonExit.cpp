#include "DungeonExit.h"

DungeonExit::DungeonExit(){}

DungeonExit::DungeonExit(sf::Vector2f pos, int* nextLevel, int levelIndex)
{
	//Load texture from path
	texture.loadFromFile("gfx/DungeonExit.png");
	setTexture(&texture);
	//Initialize gameobject variables
	setPosition(pos);
	setSize(sf::Vector2f(75, 75));
	setCollisionBox(sf::FloatRect(0, 0, getSize().x, getSize().y));
	//Initialize variables
	this->nextLevel = nextLevel;
	this->levelIndex = levelIndex;
}

DungeonExit::~DungeonExit() {}

void DungeonExit::loadLevel()
{
	//Set nextlevel to this index + 1
	*nextLevel = levelIndex + 1;
}