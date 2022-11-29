#pragma once
#include "Enemy.h"
class Gladiator : public Enemy
{
public:
	Gladiator() {}
	Gladiator(sf::Vector2f spawnPos, AudioManager* audio, int enemyID);
	~Gladiator() {};
	void update(float dt) override;
	EnemyType getEnemyType() override;
protected:
	void setupAnimations() override;
};

