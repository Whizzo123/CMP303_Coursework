#pragma once
#include "Character.h"
#include "Inventory.h"
#include "ChestManager.h"
#include "DungeonExit.h"
#include <functional>
#include "Network/NetworkingManager.h"

struct PositionRecording
{
	PositionRecording(float time, sf::Vector2f pos)
	{
		timeOfRecording = time;
		position = pos;
	}
	float timeOfRecording;
	sf::Vector2f position;
};

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
	bool DidJustAttack() { return justAttacked; }
	void ResetJustAttacked() { justAttacked = false; }
	Character* GetEnemyTarget() { return enemyTarget; }
	void manualAttack();
	void updateVelocity(sf::Vector2f newVelocity, float time);
	void calculateNewVelocity();
	sf::Vector2f normalize(sf::Vector2f vector);
	void setPlayerPosition(sf::Vector2f pos);
	sf::Vector2f lerp(sf::Vector2f vectorA, sf::Vector2f vectorB, float time);
	float lerp(float a, float b, float time);
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
	bool justAttacked;
	PositionRecording* lastPositions[2];
private:
	void attack();
protected:
	void onDeath() override;
};

