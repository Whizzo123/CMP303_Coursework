#pragma once
#include "Framework/TileMap.h"
#include "Framework/Collision.h"

class DungeonDiverTileMap : public TileMap
{

public:
	DungeonDiverTileMap() {}
	DungeonDiverTileMap(std::string mapTexture, sf::Vector2f tileSize, sf::Vector2u mapDimensions, std::vector<int> mapData);
	bool collisionCheck(GameObject* toCheckWith);
	sf::Vector2f findRandomSpawnPos();
	std::vector<sf::Vector2f> getSpawnableTiles();
private:
	//Vector of spawnable tiles
	std::vector<sf::Vector2f> spawnableTiles;
	//Dimensions of tiles
	sf::Vector2f tileSize;
};

