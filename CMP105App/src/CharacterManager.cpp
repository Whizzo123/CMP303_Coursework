#include "CharacterManager.h"

CharacterManager::CharacterManager(int maxCharacters, AudioManager* audio, DungeonDiverTileMap* map)
{
	//Initialize variables
	this->maxCharacters = maxCharacters;
	this->audio = audio;
	this->map = map;
}

/// <summary>
/// Responsible for spawning characters from network
/// </summary>
/// <param name="info"></param>
/// <param name="length"></param>
void CharacterManager::spawnNetworkEnemies(EnemyInfo* info, int length)
{
	for (int i = 0; i < length; i++)
	{
		EnemyInfo enemyInfo = info[i];
		switch (enemyInfo.enemyClass)
		{
		case IMP:
			characters.push_back(new Imp(enemyInfo.spawnPosition, audio));
			break;
		case GLADIATOR:
			characters.push_back(new Gladiator(enemyInfo.spawnPosition, audio));
			break;
		case MINOTAUR:
			characters.push_back( new Minotaur(enemyInfo.spawnPosition, audio));
			break;
		default:
			break;
		}
		characters[i]->setAlive(true);
	}
}

std::vector<Enemy*> CharacterManager::getAllCharacters()
{
	// Check have we spawned yet
	if (characters.size() == 0)
		spawnAllCharacters();
	
	return characters;
}

void CharacterManager::draw(sf::RenderWindow* window)
{
	//For all characters
	for (int i = 0; i < characters.size(); i++)
	{
		//If character is alive
 		if (characters[i]->isAlive())
		{
			window->draw(*characters[i]);
			characters[i]->drawStatIndicator(window);
		}
	}
}

void CharacterManager::spawn(std::string characterName, sf::Vector2f spawnPos)
{
	//For all characters
	for (int i = 0; i < maxCharacters; i++)
	{
		//If found dead character use to spawn new character
		if (characters[i]->isAlive() == false)
		{
			if (characterName == "Imp")
			{
				characters[i] = new Imp(spawnPos, audio);
			}
			else if (characterName == "Gladiator")
			{
				characters[i] = new Gladiator(spawnPos, audio);
			}
			else if (characterName == "Minotaur")
			{
				characters[i] = new Minotaur(spawnPos, audio);
			}
			characters[i]->setAlive(true);
			break;
		}
	}
}

void CharacterManager::spawnAllCharacters()
{
	for (int i = 0; i < maxCharacters; i++)
	{
		//Pick random number between 100 - 1
		int random = rand() % 100 + 1;
		//If less than 60 spawn imp
		if (random < 60)
			characters.push_back(new Imp(map->findRandomSpawnPos(), audio));
		//If between 60 and 84 spawn gladiator
		else if (random < 85)
			characters.push_back(new Gladiator(map->findRandomSpawnPos(), audio));
		//If above 85 spawn minotaur
		else
			characters.push_back(new Minotaur(map->findRandomSpawnPos(), audio));
		characters[i]->setAlive(true);
	}
}

void CharacterManager::update(float dt, DungeonDiverTileMap* map, Player* player)
{
	if (characters.size() != 0)
	{
		//For all characters
		for (int i = 0; i < characters.size(); i++)
		{
			//If character is alive
			if (characters[i]->isAlive())
			{
				//Check for collisions with map and player
				map->collisionCheck(characters[i]);
				characters[i]->setFollowing(ExtendedCollision::checkSphereRectCollision(player, characters[i], 200));
				//If player is within following range set target to player
				if (characters[i]->getFollowing())
					characters[i]->setFollowingTarget(player);
				else
					characters[i]->setFollowingTarget(nullptr);
				characters[i]->update(dt);
			}
		}
	}
}

Enemy* CharacterManager::checkCollisions(sf::FloatRect* fr)
{
	//For all characters
	for (int i = 0; i < characters.size(); i++)
	{
		//If character is alive
		if (characters[i]->isAlive())
		{
			//Check collision with provided float rect
			sf::FloatRect rect = characters[i]->getCollisionBox();
			if (ExtendedCollision::checkCollisionBoxes(fr, &rect))
			{
				return characters[i];
			}
		}
	}
	return nullptr;
}