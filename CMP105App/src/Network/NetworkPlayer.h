#pragma once
#include "NetworkCharacter.h"


class NetworkPlayer : public NetworkCharacter
{
public:
	NetworkPlayer() {};
	NetworkPlayer(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, AudioManager* aud, int ID, bool clientUpdateable);
	~NetworkPlayer();
	void Update(float dt);


private:
	void setupAnimations();
};