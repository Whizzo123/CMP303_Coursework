#include "NetworkCharacter.h"

NetworkCharacter::NetworkCharacter(std::string textureName, sf::Vector2f pos, sf::Vector2f size, AudioManager* aud, int ID, bool clientUpdateable)
	: NetworkObject(ID, clientUpdateable)
{
	std::string textureNameToLoad = "gfx/" + textureName + ".png";
	texture.loadFromFile(textureNameToLoad);
	setTexture(&texture);
	setPosition(pos);
	setSize(size);
	setCollisionBox(0, 0, size.x, size.y);
	alive = false;
}

NetworkCharacter::~NetworkCharacter()
{
	if(aud)
		delete aud;
}

void NetworkCharacter::UpdateAnimations(float dt)
{
	//Play current animation
	currentAnimation->animate(dt);
	//Set character texture to current frame of animation
	setTextureRect(currentAnimation->getCurrentFrame());
}
