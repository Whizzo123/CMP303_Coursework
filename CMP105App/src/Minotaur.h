#pragma once
#include "Enemy.h"
class Minotaur : public Enemy
{
public:
	Minotaur() {};
	Minotaur(sf::Vector2f spawnPos, AudioManager* audio);
	~Minotaur() {};
	void update(float dt) override;
	EnemyType getEnemyType() override;
protected:
	void setupAnimations() override;
};

