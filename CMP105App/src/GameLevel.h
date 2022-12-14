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
		sf::Vector2f spawnPoint, int levelIndex, int numberOfEnemies, int numberOfChests, std::vector<int>* mapData, sf::Vector2u mapDimensions);
	~GameLevel() {}
	void update(float dt) ;
	void handleInput(float dt) override;
	void render() override;
	void switchToLevel(std::map<int, Player*> players) override;
	std::vector<Enemy*> getEnemies() { return characterManager->getAllCharacters(); }
	int getNumOfEnemies() { return characterManager->getCurrentCharacterCount(); }
	std::vector<sf::Vector2f> GetPlayerPos();
	void handleNetwork(float dt);
	void SyncNetworkPosition(sf::Packet packet);
	void GetEnemyInfoForClient(int socketID);
	void GetChestInfoForClient(int socketID);
	void SyncNetworkPlayerPositions(sf::Packet packet);
	void PacketUpdatedNetworkObjectData();
	std::vector<NetworkObject> GetUpdatedNetworkObjects();
	void SpawnNetworkedEnemies(sf::Packet packet);
	void ServerUpdateEnemyPositions();
	void SyncNetworkEnemyPositions(sf::Packet packet);
	int GetIndexForPlayer(Player* player);
	Player* GetPlayerFromIndex(int playerID);
	void SyncPlayerAttackedEvent(sf::Packet packet);
	void HandlePlayerAttackedEvent(PlayerAttackData data);
	void HandlePlayerDeath(int playerID);
	void HandlePlayerReviveEvent(int playerID, sf::Vector2f pos);
	void SyncPlayerReviveEvent(sf::Packet packet);
	void SyncInventoryChangeEvent(sf::Packet packet);
	void HandleInventoryChangeEvent(InventorySyncData data);
	void SpawnNetworkedChests(sf::Packet packet);
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
private:
	sf::Vector2f pauseTextPos;
	sf::Vector2f skipButtonPos;
	int levelIndex;
	float currentNetworkTickTime = 0.0f;
	int _numberOfEnemies;
	int _numberOfChests; 
	std::map<int, Player*> _players;
	sf::Vector2f _spawnPoint;
};

