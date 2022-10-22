#pragma once
#include "Enemy.h"
#include "ExtendedCollision.h"
#include "Framework/VectorHelper.h"
class Imp : public Enemy
{
public:
	Imp() {}
	Imp(sf::Vector2f spawnPos, AudioManager* audio);
	~Imp() { std::cout << "Calling Imp Deconstructor" << std::endl; };
	void update(float dt) override;
protected:
	void setupAnimations() override;
};

