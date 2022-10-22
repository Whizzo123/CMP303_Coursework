#pragma once
#include "Character.h"
#include "Inventory.h"
#include "ChestManager.h"
#include "DungeonExit.h"

class Player : public Character
{
public:
	Player() {};
	Player(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, Input* input, sf::RenderWindow* window, AudioManager* audio, float maxHealth);
	~Player();
	void handleInput(float dt) override;
	void update(float dt) override;
	void collisionResponse(GameObject* other) override;
	void setupAnimations() override;
	void updateAnimations(float dt) override;
	void drawDebugInfo();
	void setEnemyTarget(Character* other);
	sf::FloatRect* getAttackRect();
	sf::FloatRect* getInteractRect();
	void setChestManager(ChestManager* chestManager);
	ChestManager* getChestManager();
	void setDungeonExit(DungeonExit* dungeonExit);
	DungeonExit* getDungeonExit();
	Inventory* getInventory();
	int* getNextLevel();
	void setNextLevel(int* nextLevel);
private:
	sf::RectangleShape attackRectDebug;
	sf::FloatRect attackRect;
	sf::FloatRect interactRect;
	Character* enemyTarget;
	ChestManager* chestManager;
	DungeonExit* dungeonExit;
	float weaponDamage;
	float attackSpeed;
	float currentAttackTimer;
	int* nextLevel;
private:
	void attack();
protected:
	void onDeath() override;
};

