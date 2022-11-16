#pragma once

#include "../NetworkObject.h"
#include "../Framework/AudioManager.h"
#include "../Framework/Animation.h"

class NetworkCharacter : public NetworkObject
{
public:
	NetworkCharacter() { alive = false; }
	NetworkCharacter(std::string textureName, sf::Vector2f pos, sf::Vector2f size, AudioManager* aud, int ID, bool clientUpdatable);
	~NetworkCharacter();

protected:
	void UpdateAnimations(float dt);

protected:
	bool alive;
	sf::Texture texture;
	AudioManager* aud;
	Animation* currentAnimation;
	Animation walkAnimation;
	Animation idleAnimation;
	Animation attackAnimation;
	Animation damageAnimation;
	Animation deathAnimation;
	
};

