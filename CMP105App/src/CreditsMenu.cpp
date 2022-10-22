#include "CreditsMenu.h"

CreditsMenu::CreditsMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	titleFont.loadFromFile("font/arial.ttf");
	background.setPosition(sf::Vector2f(0, 0));
	background.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
	backgroundTexture.loadFromFile("gfx/MainMenuBack.png");
	background.setTexture(&backgroundTexture);
	backButton = new LevelLoaderButton("backButton", sf::Vector2f(0, 600), sf::Vector2f(150, 75), input, window, 0);
	titleText.setFont(titleFont);
	titleText.setString("Credits");
	titleText.setPosition(sf::Vector2f(500, 10));
	creditsText.setFont(titleFont);
	creditsText.setString("Elthen (Ahmet Avci) artist for Characters and dungeon tile set \nBlarumyrran artist for treasure chest sprite \nwww.FesliyanStudios.com for dungeon music \nmixkit.co for damage and attack sound effects \nInspectorJ for door sound effects");
	creditsText.setPosition(sf::Vector2f(250, 80));
}

void CreditsMenu::handleInput(float dt)
{
	//Check for input on backbutton
	backButton->handleInput(dt);
}

void CreditsMenu::update(float dt)
{
	//Update back button
	backButton->update(dt);
	//If back button was clicked move us back to main menu
	if (backButton->isClicked())
	{
		nextLevel = backButton->getLevelIndex();
		backButton->reset();
	}
}

void CreditsMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(*backButton);
	window->draw(titleText);
	window->draw(creditsText);
	endDraw();
}