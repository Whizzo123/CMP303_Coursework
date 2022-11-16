#include "NetworkPlayer.h"

NetworkPlayer::NetworkPlayer(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, AudioManager* aud, int ID, bool clientUpdateable)
	: NetworkCharacter(textureName, pos, size, aud, ID, clientUpdateable)
{
	setupAnimations();
}

NetworkPlayer::~NetworkPlayer()
{

}

void NetworkPlayer::Update(float dt)
{
	UpdateAnimations(dt);
}

void NetworkPlayer::setupAnimations()
{
	for (int i = 0; i < 4; i++)
	{
		walkAnimation.addFrame(sf::IntRect(32 * i, 0, 32, 32));
	}
	walkAnimation.setFrameSpeed(0.2f);

	for (int i = 0; i < 4; i++)
	{
		idleAnimation.addFrame(sf::IntRect(32 * i, 32, 32, 32));
	}
	idleAnimation.setFrameSpeed(0.25f);

	for (int i = 0; i < 7; i++)
	{
		attackAnimation.addFrame(sf::IntRect(32 * i, 64, 32, 32));
	}
	attackAnimation.setLooping(false);
	attackAnimation.setFrameSpeed(0.2f);

	for (int i = 0; i < 4; i++)
	{
		damageAnimation.addFrame(sf::IntRect(32 * i, 96, 32, 32));
	}
	damageAnimation.setLooping(false);
	damageAnimation.setFrameSpeed(0.25f);

	for (int i = 0; i < 7; i++)
	{
		deathAnimation.addFrame(sf::IntRect(32 * i, 128, 32, 32));
	}
	deathAnimation.setLooping(false);
	deathAnimation.setFrameSpeed(0.2f);
	currentAnimation = &idleAnimation;
}