#pragma once
#include "Character.h"
#include "Framework/VectorHelper.h"

enum EnemyType { IMP, GLADIATOR, MINOTAUR };


struct EnemyInfo
{
	sf::Int32 objectID;
	EnemyType enemyClass;
	sf::Vector2f spawnPosition;
	sf::Vector2f patrolPosition;
};


class Enemy : public Character
{
public:
	Enemy() {};
	Enemy(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, float maxHealth, float attackCooldown, float weaponDamage, float attackRange, AudioManager* audio);
	~Enemy() {};
	bool getFollowing() { return following; }
	void setFollowing(bool value) { following = value; }
	Character* getFollowingTarget() { return followingTarget; }
	void setFollowingTarget(Character* target) { followingTarget = target; }
	void update(float dt) override;
	virtual EnemyType getEnemyType() { return _myType; }
	void setMoveDirection(sf::Vector2f moveDirection);
	sf::Vector2f getMoveDirection();
	void setIsServer(bool value) { isServer = value; }
protected:
	void updateMovement(float dt);
	void updateAnimations(float dt) override;
	void collisionResponse(GameObject* other) override;
	void attack(Character* character);
	void onDeath() override;
protected:
	float moveTimer;
	float currentMoveTimer;
	float breakTimer;
	float currentBreakTimer;
	float attackCooldown;
	float currentAttackCooldown;
	float weaponDamage;
	float attackRange;
	bool onBreak;
	bool following;
	bool attacking;
	bool isServer;
	Character* followingTarget;
	EnemyType _myType;
	sf::Vector2f currentMoveDirection;
};

