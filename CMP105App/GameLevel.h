#pragma once
#include "Level.h"
#include "LevelLoaderButton.h"
#include "BackgroundMap.h"
#include <iostream>

class GameLevel : public Level
{
public:
	GameLevel() {};
	GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, Player* player, sf::Vector2f playerSpawnPos,
		int levelIndex, int numberOfEnemies, int numberOfChests, std::vector<int>* mapData, sf::Vector2u mapDimensions);
	~GameLevel() {}
	void update(float dt) ;
	void handleInput(float dt) override;
	void render() override;
	void switchToLevel() override;
protected:
	void spawnInEntities(int numberOfEnemies, int numberOfChests);
protected:
	Player* player;
	DungeonDiverTileMap* map;
	BackgroundMap* backgroundMap;
	Background* background;
	CharacterManager* characterManager;
	Inventory* inventory;
	InventoryManager* inventoryManager;
	DragController* dragController;
	ChestManager* chestManager;
	DungeonExit* dungeonExit;
	HealthUI* healthUI;
	Cursor* cursor;
	sf::Text pauseText;
	sf::Font pauseFont;
	LevelLoaderButton* skipLevelButton;
private:
	sf::Vector2f pauseTextPos;
	sf::Vector2f skipButtonPos;
	int levelIndex;
	sf::Vector2f playerSpawnPos;
};

