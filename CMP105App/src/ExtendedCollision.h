#pragma once
#include "Framework/Collision.h"
class ExtendedCollision : public Collision
{
public:
	static bool checkCollisionBoxes(sf::FloatRect* fr1, sf::FloatRect* fr2);
	static bool checkSphereRectCollision(GameObject* rect, GameObject* circle, float radius);
};

