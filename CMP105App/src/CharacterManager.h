#pragma once

#include "SFML\Graphics.hpp"
#include "Imp.h"
#include "Gladiator.h"
#include "Minotaur.h"
#include <vector>
#include "ExtendedCollision.h"
#include "Player.h"
class CharacterManager
{

public:
	CharacterManager() {};
	CharacterManager(int maxCharacters, AudioManager* audio, DungeonDiverTileMap* map);
	~CharacterManager() { };
	void draw(sf::RenderWindow* window);
	void spawn(std::string characterName, sf::Vector2f pos);
	void update(float dt, DungeonDiverTileMap* map, Player* player);
	Enemy* checkCollisions(sf::FloatRect* fr);
private:
	//Vector of all enemies in level
	std::vector<Enemy*> characters;
	//Max enemies for level
	int maxCharacters;
	//Reference to audiomanager
	AudioManager* audio;
	//Reference to current tilemap for level
	DungeonDiverTileMap* map;
};

