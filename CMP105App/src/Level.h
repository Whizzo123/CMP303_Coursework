#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Background.h"
#include "CharacterManager.h"
#include "Inventory.h"
#include "InventoryManager.h"
#include "DragController.h"
#include "Weapon.h"
#include "ChestManager.h"
#include "DungeonExit.h"
#include "HealthUI.h"
#include "Cursor.h"
#include "Network/NetworkPlayer.h"

class Level : public BaseLevel{
public:
	Level() {};
	Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~Level();

	void handleInput(float dt) override;
	void update(float dt);
	void render();
	void resetNextLevel();
	int getNextLevel();
	virtual void switchToLevel(std::map<int, Player*> players) {};
	void setNextLevel(int levelNumber) { nextLevel = levelNumber; };
	//void setPlayer(Player* player) { this->player = player; }
protected:
	int nextLevel;
	//Player* player;
};