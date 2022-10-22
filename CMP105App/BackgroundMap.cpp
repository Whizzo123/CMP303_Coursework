#include "BackgroundMap.h"

BackgroundMap::BackgroundMap(std::string textureName, sf::Vector2f tileSize, sf::Vector2u mapDimensions)
{
	//Set texPath to path to texture
	std::string texPath = "gfx/" + textureName + ".png";
	//Reference char pointer to string
	const char* textureString = texPath.c_str();
	//Load texture from path
	loadTexture(textureString);

	GameObject tile;
	//Set size of tiles
	tile.setSize(tileSize);
	//Add this tile to back of vector
	tileSet.push_back(tile);
	//Set texture rect
	tileSet[0].setTextureRect(sf::IntRect(288, 0, 32, 32));
	//Set mapDimensions
	this->mapDimensions = mapDimensions;
	//Construct tilemap to provided dimensions
	for (int i = 0; i < mapDimensions.x; i++)
	{
		for (int j = 0; j < mapDimensions.y; j++)
		{
			tileMap.push_back(0);
		}
	}
	//Set position of map
	setPosition(sf::Vector2f(-1000, -1000));
	//Build it
	buildLevel();
}