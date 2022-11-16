#pragma once
#include "Level.h"
#include "LevelLoaderButton.h"
#include "BackgroundMap.h"
#include <iostream>

class GameLevel : public Level
{
public:
	GameLevel() {};
	GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, Player* player, 
		int levelIndex, int numberOfEnemies, int numberOfChests, std::vector<int>* mapData, sf::Vector2u mapDimensions);
	~GameLevel() {}
	void update(float dt) ;
	void handleInput(float dt) override;
	void render() override;
	void switchToLevel(Player* player, std::vector<NetworkPlayer*> otherPlayers, bool isServer) override;
	void spawnInEntities(EnemyInfo* info, int enemyInfoLength);
	std::vector<Enemy*> getEnemies() { return characterManager->getAllCharacters(); }
	int getNumOfEnemies() { return characterManager->getCurrentCharacterCount(); }
	void spawnInEntities();
protected:
	
protected:
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
	std::vector<NetworkPlayer*> _otherPlayers;
	int _numberOfEnemies;
	int _numberOfChests;
private:
	sf::Vector2f pauseTextPos;
	sf::Vector2f skipButtonPos;
	int levelIndex;
};

