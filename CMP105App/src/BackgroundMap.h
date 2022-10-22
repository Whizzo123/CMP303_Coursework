#pragma once
#include "Framework/TileMap.h"
class BackgroundMap : public TileMap
{
public:
	BackgroundMap(std::string textureName, sf::Vector2f tileSize, sf::Vector2u mapDimensions);
};

