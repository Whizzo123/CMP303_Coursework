#pragma once
#include "Framework/GameObject.h"
#include"Framework/Animation.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Inventory.h"
#include "Framework/VectorHelper.h"
#include "Framework/AudioManager.h"

class Character : public GameObject
{
public:
	Character() { alive = false; };
	Character(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, int maxHealth, AudioManager* audio);
	~Character() {};
	void update(float dt) override;
	void heal(int healPoints);
	void damage(double damage, sf::Vector2f enemyPos);
	void drawStatIndicator(sf::RenderWindow* window);
	void setInventory(Inventory* inventory);
	float getCurrentHealth();
	float getMaxHealth();
	void resetHealth();
protected:
	void debugLog(std::string msg);
	virtual void updateAnimations(float dt);
	virtual void setupAnimations() {};
	virtual void collisionResponse(GameObject* other);
	virtual void onDeath() {};
private:
	void calculateKnockback(sf::Vector2f otherPos);
protected:
	//Character texture
	sf::Texture texture;
	//Damage text
	sf::Text statsIndicator;
	sf::Font font;
	float speed;
	bool moving;
	bool dead;
	//Time for damage text to display
	float statShowingTimer;
	float currentStatShowingTimer;
	sf::Vector2f lastDirection;
	bool showingStatsIndicator;
	//Character animations
	Animation* currentAnimation;
	Animation walkAnimation;
	Animation idleAnimation;
	Animation attackAnimation;
	Animation deathAnimation;
	Animation damageAnimation;
	std::string characterName;
	int maxHealth;
	float currentHealth;
	Inventory* inventory;
	AudioManager* audio;
	std::string damageAudioName;
};

