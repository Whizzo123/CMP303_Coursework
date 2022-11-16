#include "Minotaur.h"

Minotaur::Minotaur(sf::Vector2f spawnPos, AudioManager* audio) 
	: Enemy("Minotaur", spawnPos, sf::Vector2f(150, 150), 125.0f, 4, 2, 2, 150, audio)
{
	//Initialize variables
	setupAnimations();
	characterName = "Minotaur";
	moveTimer = 3.0f;
	currentMoveTimer = 0.0f;
	onBreak = true;
	breakTimer = 2.0f;
	currentBreakTimer = 2.0f;
	this->damageAudioName = "MinotaurDamage";
	_myType = MINOTAUR;
}

void Minotaur::update(float dt)
{
	Enemy::update(dt);
}

//Setup minotaur animations
void Minotaur::setupAnimations()
{
	idleAnimation = Animation();
	for (int i = 0; i < 5; i++)
	{
		idleAnimation.addFrame(sf::IntRect(80 * i, 0, 80, 80));
	}
	idleAnimation.setFrameSpeed(0.25f);
	for (int i = 0; i < 8; i++)
	{
		walkAnimation.addFrame(sf::IntRect(80 * i, 80, 80, 80));
	}
	walkAnimation.setFrameSpeed(0.25f);
	for (int i = 0; i < 9; i++)
	{
		attackAnimation.addFrame(sf::IntRect(80 * i, 160, 80, 80));
	}
	attackAnimation.setFrameSpeed(0.25f);
	for (size_t i = 0; i < 3; i++)
	{
		damageAnimation.addFrame(sf::IntRect(80 * i, 240, 80, 80));
	}
	damageAnimation.setFrameSpeed(0.15f);
	deathAnimation.setLooping(false);
	for (int i = 0; i < 6; i++)
	{
		deathAnimation.addFrame(sf::IntRect(80 * i, 320, 80, 80));
	}
	deathAnimation.setFrameSpeed(0.2f);
	currentAnimation = &idleAnimation;
}

EnemyType Minotaur::getEnemyType()
{
	return _myType;
}