#include "WinMenu.h"

WinMenu::WinMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	titleFont.loadFromFile("font/arial.ttf");
	background.setPosition(0, 0);
	background.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
	backgroundTexture.loadFromFile("gfx/MainMenuBack.png");
	background.setTexture(&backgroundTexture);
	titleText.setFont(titleFont);
	titleText.setString("You've Completed The Dungeon");
	titleText.setPosition(sf::Vector2f(475, 150) + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
}

void WinMenu::handleInput(float dt)
{

}

void WinMenu::update(float dt)
{
	//Set menu elements with respect to view
	titleText.setPosition(sf::Vector2f(475, 150) + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
	background.setPosition(sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2,
		window->getView().getCenter().y - window->getSize().y / 2));
}

void WinMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(titleText);
	endDraw();
}