#pragma once
#include "Level.h"
#include "LevelLoaderButton.h"
#include "BackgroundMap.h"
#include <iostream>
#include "Network/NetworkingManager.h"

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
	void switchToLevel() override;
	std::vector<Enemy*> getEnemies() { return characterManager->getAllCharacters(); }
	int getNumOfEnemies() { return characterManager->getCurrentCharacterCount(); }
	std::vector<sf::Vector2f> GetPlayerPos();
	void handleNetwork(float dt);
	void SyncNetworkPosition(NetworkObjectUpdateData updatedObjects);
	void GetEnemyInfoForClient(int socketID);
	void SyncNetworkPlayerPositions(std::vector<sf::Vector2f> positions);
	void PacketUpdatedNetworkObjectData();
	std::vector<NetworkObject> GetUpdatedNetworkObjects();
	void SpawnNetworkedEnemies(EnemySpawnInfoResult result);
	void ServerUpdateEnemyPositions();
	void SyncNetworkEnemyPositions(NetworkObjectUpdateData data);
	int GetIndexForPlayer(Player* player);
	Player* GetPlayerFromIndex(int playerID);
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
	std::map<int, Player*> _otherPlayers;
	int _numberOfEnemies;
	int _numberOfChests;
private:
	sf::Vector2f pauseTextPos;
	sf::Vector2f skipButtonPos;
	int levelIndex;
	float currentNetworkTickTime = 0.0f;
};

