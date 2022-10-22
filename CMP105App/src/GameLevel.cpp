#include "GameLevel.h"

GameLevel::GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, Player* player, sf::Vector2f playerSpawnPos,
	int levelIndex, int numberOfEnemies, int numberOfChests, std::vector<int>* mapData, sf::Vector2u mapDimensions) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	this->player = player;
	this->levelIndex = levelIndex;
	this->playerSpawnPos = playerSpawnPos;
	inventoryManager = new InventoryManager();
	dragController = new DragController(input, inventoryManager, window);
	chestManager = new ChestManager(input, inventoryManager, audio);
	background = new Background(window, player);
	inventoryManager->addToInventories(player->getInventory());
	healthUI = new HealthUI(player, window, sf::Vector2f(window->getSize().x - 150.0f, window->getSize().y - 40.0f));
	cursor = new Cursor(window, inventoryManager, input, player);
	map = new DungeonDiverTileMap("DungeonWall_V2", sf::Vector2f(75, 75), mapDimensions, *mapData);
	backgroundMap = new BackgroundMap("DungeonWall_V2", sf::Vector2f(75, 75), sf::Vector2u(100, 100));
	spawnInEntities(numberOfEnemies, numberOfChests);
	pauseFont.loadFromFile("font/arial.ttf");
	pauseText.setFont(pauseFont);
	pauseText.setString("Paused");
	pauseTextPos = sf::Vector2f(550, 150);
	skipButtonPos = sf::Vector2f(900, 500);
	pauseText.setPosition(pauseTextPos);
	skipLevelButton = new LevelLoaderButton("skipButton", skipButtonPos, sf::Vector2f(150, 75), input, window, 0);
}

void GameLevel::handleInput(float dt)
{
	//Handle pause menu 
	skipLevelButton->handleInput(dt);
	if (input->isPressed(sf::Keyboard::Tab))
	{
		if (gameState->getCurrentState() != State::PAUSE)
			gameState->setCurrentState(State::PAUSE);
		else
			gameState->setCurrentState(State::LEVEL);
	}
	//Handle player and cursor input
	if (gameState->getCurrentState() != State::PAUSE)
	{
		player->handleInput(dt);
		cursor->handleInput();
	}
}

void GameLevel::spawnInEntities(int numberOfEnemies, int numberOfChests)
{
	dungeonExit = new DungeonExit(map->getSpawnableTiles().back(), &nextLevel, levelIndex);
	characterManager = new CharacterManager(numberOfEnemies, audio, map);
	chestManager->spawn(map, window, input, numberOfChests);
}

void GameLevel::update(float dt)
{
	skipLevelButton->update(dt);
	//If skip level button clicked set next level 
	if (skipLevelButton->isClicked())
	{
		nextLevel = levelIndex + 1;
		skipLevelButton->reset();
	}
	//Set pause menu elements positions with respect to view position
	pauseText.setPosition(pauseTextPos + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
	skipLevelButton->setPosition(skipButtonPos + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
	if (gameState->getCurrentState() != State::PAUSE)
	{
		//Check for map collisions
		map->collisionCheck(player);
		//Update characters and player
		characterManager->update(dt, map, player);
		player->update(dt);
		player->setEnemyTarget(characterManager->checkCollisions(player->getAttackRect()));
		background->update(dt);
		inventoryManager->update(dt);
		dragController->update(dt);
		healthUI->update();
		cursor->update();
	}
}

void GameLevel::render()
{
	beginDraw();
	backgroundMap->render(window);
	map->render(window);
	chestManager->draw(window);
	window->draw(*dungeonExit);
	characterManager->draw(window);
	player->drawDebugInfo();
	if (player->isAlive())
	{
		window->draw(*player);
		player->drawStatIndicator(window);
	}
	inventoryManager->draw(window);
	healthUI->draw();
	cursor->draw();
	if (gameState->getCurrentState() == State::PAUSE)
	{
		window->draw(pauseText);
		window->draw(*skipLevelButton);
	}
	endDraw();
}

void GameLevel::switchToLevel()
{
	player->setChestManager(chestManager);
	player->setDungeonExit(dungeonExit);
	player->setPosition(playerSpawnPos);
	player->setNextLevel(&nextLevel);
}