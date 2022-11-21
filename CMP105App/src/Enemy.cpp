#include "Enemy.h"

Enemy::Enemy(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, float maxHealth, float attackCooldown, float weaponDamage,
	float attackRange, AudioManager* audio) : Character(textureName, pos, size, speed, maxHealth, audio)
{
	//Initialize variables
	following = false;
	attacking = false;
	this->attackCooldown = attackCooldown;
	currentAttackCooldown = 0.0f;
	this->weaponDamage = weaponDamage;
	this->attackRange = attackRange;
	this->currentMoveDirection = sf::Vector2f(0.0f, 0.0f);
}

void Enemy::update(float dt)
{
	Character::update(dt);
	//If not dead
	if (!dead)
	{
		//If enemy has following target
		if (followingTarget != nullptr)
		{
			//If target is within attack range of enemy
			if (pow(followingTarget->getPosition().x - getPosition().x, 2) + pow(followingTarget->getPosition().y - getPosition().y, 2) <= pow(attackRange, 2))
				attacking = true;
			else
				attacking = false;
		}
		//If not following a target
		if (!following)
			updateMovement(dt);
		//If not attacking
		else if (!attacking)
		{
			//Grab vector to follow target
			sf::Vector2f targetVector = VectorHelper::normalise(sf::Vector2f(followingTarget->getPosition().x - getPosition().x, followingTarget->getPosition().y - getPosition().y));
			moving = true;
			move(targetVector * speed * dt);
			lastDirection = targetVector;
		}
		//If attacking and cool down as reached 0
		else if (attacking && currentAttackCooldown <= 0)
		{
			attack(followingTarget);
			currentAttackCooldown = attackCooldown;
		}
		//If attack cooldown is greater than 0
		if (currentAttackCooldown > 0)
		{
			currentAttackCooldown -= dt;
		}
	}
	updateAnimations(dt);
}

void Enemy::updateMovement(float dt)
{
	//If break timer is greater than 0 decrement by delta time
	if (currentBreakTimer > 0)
	{
		currentBreakTimer -= dt;
	}
	else if (onBreak)
	{
		//Get random vector 
		velocity = currentMoveDirection;
		currentMoveTimer = moveTimer;
		//End break
		moving = true;
		onBreak = false;
	}
	//if timer is greater than 0 and not on break
	if (currentMoveTimer > 0 && !onBreak)
	{
		//Move on vector
		move(velocity * (50.0f * dt));
		lastDirection = velocity;
		currentMoveTimer -= dt;
	}
	//If timer is less than 0 and not on break
	else if (currentMoveTimer < 0 && !onBreak)
	{
		//Set velocity to 0
		velocity = sf::Vector2f(0, 0);
		currentMoveTimer = moveTimer;
		//Start break
		if (isServer)
		{
			float randomX = -1 + static_cast<float> (rand() / (static_cast<float>(RAND_MAX / (1 - (-1)))));
			float randomY = -1 + static_cast<float> (rand() / (static_cast<float>(RAND_MAX / (1 - (-1)))));
			currentMoveDirection = sf::Vector2f(randomX, randomY);
		}
		onBreak = true;
		moving = false;
		currentBreakTimer = breakTimer;
	}
}

void Enemy::setMoveDirection(sf::Vector2f newMoveDirection)
{
	currentMoveDirection = newMoveDirection;
}

sf::Vector2f Enemy::getMoveDirection()
{
	return currentMoveDirection;
}

void Enemy::attack(Character* character)
{
	//If enemy is not dead
	if (!dead)
	{
		moving = false;
		//Play attack animation
		if (lastDirection.x < 0)
			attackAnimation.setFlipped(true);
		else
			attackAnimation.setFlipped(false);
		attackAnimation.reset();
		currentAnimation = &attackAnimation;
		currentAnimation->play(true);
		//Damage character target
		character->damage(weaponDamage, getPosition());
	}
}

void Enemy::onDeath()
{
	debugLog("I'm dead");
}

void Enemy::updateAnimations(float dt)
{
	Character::updateAnimations(dt);
}

void Enemy::collisionResponse(GameObject* other)
{
	Character::collisionResponse(other);
}