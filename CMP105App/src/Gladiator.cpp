#include "Gladiator.h"

Gladiator::Gladiator(sf::Vector2f spawnPos, AudioManager* audio, int enemyID) 
	: Enemy("Gladiator-Sprite Sheet", spawnPos, sf::Vector2f(100, 100), 150.0f, 3, 1.5, 2, 150, audio, enemyID)
{
	//Initialize variables
	setupAnimations();
	characterName = "Gladiator";
	moveTimer = 3.0f;
	currentMoveTimer = 0.0f;
	onBreak = true;
	breakTimer = 2.0f;
	currentBreakTimer = 2.0f;
	this->damageAudioName = "GladiatorDamage";
	_myType = GLADIATOR;
}

void Gladiator::update(float dt)
{
	Enemy::update(dt);
}

//Setup animations for gladiator
void Gladiator::setupAnimations()
{
	idleAnimation = Animation();
	for (int i = 0; i < 5; i++)
	{
		idleAnimation.addFrame(sf::IntRect(32 * i, 0, 32, 32));
	}
	idleAnimation.setFrameSpeed(0.25f);
	for (int i = 0; i < 8; i++)
	{
		walkAnimation.addFrame(sf::IntRect(32 * i, 32, 32, 32));
	}
	walkAnimation.setFrameSpeed(0.25f);
	for (int i = 0; i < 7; i++)
	{
		attackAnimation.addFrame(sf::IntRect(32 * i, 64, 32, 32));
	}
	attackAnimation.setFrameSpeed(0.25f);
	for (int i = 0; i < 4; i++)
	{
		damageAnimation.addFrame(sf::IntRect(32 * i, 96, 32, 32));
	}
	damageAnimation.setFrameSpeed(0.25f);
	deathAnimation.setLooping(false);
	for (int i = 0; i < 6; i++)
	{
		deathAnimation.addFrame(sf::IntRect(32 * i, 128, 32, 32));
	}
	deathAnimation.setFrameSpeed(0.2f);
	deathAnimation.setLooping(false);
	currentAnimation = &idleAnimation;
}

EnemyType Gladiator::getEnemyType()
{
	return _myType;
}