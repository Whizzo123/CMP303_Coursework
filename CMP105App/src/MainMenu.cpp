#include "MainMenu.h"

MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	titleFont.loadFromFile("font/arial.ttf");
	background.setPosition(0, 0);
	background.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
	backgroundTexture.loadFromFile("gfx/MainMenuBack.png");
	background.setTexture(&backgroundTexture);
	//playButton = new LevelLoaderButton("playButton", sf::Vector2f(500, 300), sf::Vector2f(150, 75), input, window, 2);
	optionsButton = new LevelLoaderButton("optionsButton", sf::Vector2f(500, 400), sf::Vector2f(150, 75), input, window, 1);
	creditsButton = new LevelLoaderButton("creditsButton", sf::Vector2f(500, 500), sf::Vector2f(150, 75), input, window, 9);
	hostButton = new NetworkButton("hostButton", sf::Vector2f(400, 300), sf::Vector2f(150, 75), input, window);
	joinButton = new NetworkButton("joinButton", sf::Vector2f(600, 300), sf::Vector2f(150, 75), input, window);
	titleText.setFont(titleFont);
	titleText.setString("Dungeon Divers");
	titleText.setPosition(sf::Vector2f(475, 150));
	_startClient = false;
	_startServer = false;
}

void MainMenu::handleInput(float dt)
{
	//Handle button input
	//playButton->handleInput(dt);
	hostButton->handleInput(dt);
	joinButton->handleInput(dt);
	optionsButton->handleInput(dt);
	creditsButton->handleInput(dt);
}

void MainMenu::update(float dt)
{
	//Update buttons
	//playButton->update(dt);
	hostButton->update(dt);
	joinButton->update(dt);
	optionsButton->update(dt);
	creditsButton->update(dt);
	//if (playButton->isClicked())
	//{
		//nextLevel = playButton->getLevelIndex();
		//playButton->reset();
	//}
	if (optionsButton->isClicked())
	{
		nextLevel = optionsButton->getLevelIndex();
		optionsButton->reset();
	}
	if (creditsButton->isClicked())
	{
		nextLevel = creditsButton->getLevelIndex();
		creditsButton->reset();
	}
	if (hostButton->isClicked())
	{
		_startServer = true;
	}
	if (joinButton->isClicked())
	{
		_startClient = true;
	}
}

void MainMenu::render()
{
	beginDraw();
	window->draw(background);
	//window->draw(*playButton);
	window->draw(*hostButton);
	window->draw(*joinButton);
	window->draw(*optionsButton);
	window->draw(*creditsButton);
	window->draw(titleText);
	endDraw();
}