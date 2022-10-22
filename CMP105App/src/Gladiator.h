#pragma once
#include "Enemy.h"
class Gladiator : public Enemy
{
public:
	Gladiator() {}
	Gladiator(sf::Vector2f spawnPos, AudioManager* audio);
	~Gladiator() {};
	void update(float dt) override;

protected:
	void setupAnimations() override;
};

