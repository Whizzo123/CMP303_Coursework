#pragma once
#include "Framework/GameObject.h"
class DungeonExit : public GameObject
{
public:
	DungeonExit();
	DungeonExit(sf::Vector2f position, int* nextLevel, int levelIndex);
	~DungeonExit();
	void loadLevel();

private:
	//Dungeon exit texture
	sf::Texture texture;
	//Reference to nextLevel int
	int* nextLevel;
	//Level index
	int levelIndex;
};

