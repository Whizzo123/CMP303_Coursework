#include "DungeonDiverTileMap.h"

DungeonDiverTileMap::DungeonDiverTileMap(std::string mapTexture, sf::Vector2f tileSize, sf::Vector2u mapDimensions, std::vector<int> mapData)
{
	//Load texture from path
	std::string texPath = "gfx/" + mapTexture + ".png";
	const char* textureString = texPath.c_str();
	loadTexture(textureString);
	GameObject tile;
	//loop 31 times
	for (int i = 0; i < 31; i++)
	{
		//Setup tile
		tile.setSize(tileSize);
		tile.setCollisionBox(0, 0, tile.getSize().x, tile.getSize().y);
		tile.setCollider(true);
		tileSet.push_back(tile);
	}

	int i = 1;
	//Set up tileset from provided sprite sheet
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			//Number 12 is our floor sprite so is spawnable
			if (i == 12)
				tileSet[i].setCollider(false);
			else
				tileSet[i].setCollider(true);
			tileSet[i].setTextureRect(sf::IntRect(x * 32, y * 32, 32, 32));
			i++;
		}
	}

	this->mapDimensions = mapDimensions;

	this->tileMap = mapData;

	setPosition(sf::Vector2f(0, 0));

	buildLevel();

	//Collect all spawnable tiles in the map
	for (int x = 0; x < mapDimensions.x; x++)
	{
		for (int y = 0; y < mapDimensions.y; y++)
		{
			if (tileMap[(x * mapDimensions.y) + y] == 12)
			{
				sf::Vector2f pos = levelAssets[(x * mapDimensions.y) + y].getPosition();
				spawnableTiles.push_back(pos);
			}
		}
	}
}

bool DungeonDiverTileMap::collisionCheck(GameObject* toCheckWith)
{
	//For all tiles in map
	for (int i = 0; i < levelAssets.size(); i++)
	{
		//If it has a collider
		if (levelAssets[i].isCollider())
		{
			//Check for collisions with toCheckWith
			if (Collision::checkBoundingBox(&levelAssets[i], toCheckWith))
			{
				toCheckWith->collisionResponse(&levelAssets[i]);
			}
		}
	}
	return false;
}

sf::Vector2f DungeonDiverTileMap::findRandomSpawnPos()
{
	return spawnableTiles[rand() % spawnableTiles.size()];
}

std::vector<sf::Vector2f> DungeonDiverTileMap::getSpawnableTiles()
{
	return spawnableTiles;
}