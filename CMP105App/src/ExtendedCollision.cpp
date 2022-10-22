#include "ExtendedCollision.h"


bool ExtendedCollision::checkCollisionBoxes(sf::FloatRect* fr1, sf::FloatRect* fr2)
{
	if (fr1->left + fr1->width < fr2->left)
		return false;
	if (fr1->left > fr2->left + fr2->width)
		return false;
	if (fr1->top + fr1->height < fr2->top)
		return false;
	if (fr1->top > fr2->top + fr2->height)
		return false;

	return true;
}
bool ExtendedCollision::checkSphereRectCollision(GameObject* rect, GameObject* circle, float radiusSize)
{
	//Get circle position
	float circleX = circle->getPosition().x + (circle->getSize().x / 2);
	float circleY = circle->getPosition().y + (circle->getSize().x / 2);
	float testX = circleX;
	float testY = circleY;

	if (circleX < rect->getCollisionBox().left)
		testX = rect->getCollisionBox().left;
	else if (circleX > rect->getCollisionBox().left + rect->getCollisionBox().width)
		testX = rect->getCollisionBox().left + rect->getCollisionBox().width;

	if (circleY < rect->getCollisionBox().top)
		testY = rect->getCollisionBox().top;
	else if (circleY > rect->getCollisionBox().top + rect->getCollisionBox().height)
		testY = rect->getCollisionBox().top + rect->getCollisionBox().height;

	float distX = circleX - testX;
	float distY = circleY - testY;
	float distance = sqrt(pow(distX, 2) + pow(distY, 2));
	//If distance is within or equal to radius then has collided
	if (distance <= radiusSize)
		return true;
	return false;
}