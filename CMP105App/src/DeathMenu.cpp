#include "DeathMenu.h"

DeathMenu::DeathMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	titleFont.loadFromFile("font/arial.ttf");
	background.setPosition(0, 0);
	background.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
	backgroundTexture.loadFromFile("gfx/MainMenuBack.png");
	background.setTexture(&backgroundTexture);
	quitButton = new LevelLoaderButton("quitButton", sf::Vector2f(500, 400), sf::Vector2f(150, 75), input, window, 1);
	titleText.setFont(titleFont);
	titleText.setString("You Died");
	titleText.setPosition(sf::Vector2f(475, 150) + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
}

void DeathMenu::handleInput(float dt)
{
	//Handle input on quit button
	quitButton->handleInput(dt);
}

void DeathMenu::update(float dt)
{
	quitButton->update(dt);
	//If quit button clicked exit program
	if (quitButton->isClicked())
	{
		exit(0);
	}
	//Set menu element position with respect to view position
	background.setPosition(sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
	quitButton->setPosition(sf::Vector2f(500, 400) + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
	titleText.setPosition(sf::Vector2f(475, 150) + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
}

void DeathMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(*quitButton);
	window->draw(titleText);
	endDraw();
}