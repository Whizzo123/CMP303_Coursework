#pragma once

#include "SFML\Graphics.hpp"
#include "Imp.h"
#include "Gladiator.h"
#include "Minotaur.h"
#include <vector>
#include "ExtendedCollision.h"
#include "Player.h"

struct EnemyInfo
{
	sf::Int32 objectID;
	EnemyType enemyClass;
	sf::Vector2f spawnPosition;
	sf::Vector2f patrolPosition;
};

class CharacterManager
{

public:
	CharacterManager() {};
	CharacterManager(int maxCharacters, AudioManager* audio, DungeonDiverTileMap* map);
	~CharacterManager() { };
	void draw(sf::RenderWindow* window);
	void spawn(std::string characterName, sf::Vector2f pos);
	void spawnAllCharacters();
	void update(float dt, DungeonDiverTileMap* map, Player* player);
	Enemy* checkCollisions(sf::FloatRect* fr);
	void spawnNetworkEnemies(EnemyInfo* info, int length);
	std::vector<Enemy*> getAllCharacters();
	int getCurrentCharacterCount() { return characters.size(); }
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





