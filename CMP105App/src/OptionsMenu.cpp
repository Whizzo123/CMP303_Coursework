#include "OptionsMenu.h"

OptionsMenu::OptionsMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud) : Level(hwnd, in, gs, aud)
{
	//Initialize variables
	titleFont.loadFromFile("font/arial.ttf");
	background.setPosition(0, 0);
	background.setSize(sf::Vector2f(window->getSize().x, window->getSize().y));
	backgroundTexture.loadFromFile("gfx/MainMenuBack.png");
	background.setTexture(&backgroundTexture);
	backButton = new LevelLoaderButton("backButton", sf::Vector2f(0, 600), sf::Vector2f(150, 75), input, window, 0);
	musicOn = new Button("musicOnButton", sf::Vector2f(400, 150), sf::Vector2f(150, 75), input, window);
	musicOff = new Button("musicOffButton", sf::Vector2f(550, 150), sf::Vector2f(150, 75), input, window);
	titleText.setFont(titleFont);
	titleText.setString("Options");
	titleText.setPosition(sf::Vector2f(500, 10));
	instructionText.setFont(titleFont);
	instructionText.setString("Instruction: Use WASD to move and space bar to attack. In order to access chests \n and your inventory press E and drag and drop items between to equip better \n weapons and armour. In order to consume potions right click on their slot once they \n are in your inventory. Tab is for pausing the game. The E key is also used for the\ndungeon doors.");
	instructionText.setPosition(sf::Vector2f(50, 225));
}

void OptionsMenu::handleInput(float dt)
{
	//Handle button input
	backButton->handleInput(dt);
	musicOn->handleInput(dt);
	musicOff->handleInput(dt);
}

void OptionsMenu::update(float dt)
{
	//Update menu buttons
	backButton->update(dt);
	musicOn->update(dt);
	musicOff->update(dt);
	if (backButton->isClicked())
	{
		nextLevel = backButton->getLevelIndex();
		backButton->reset();
	}
	if (musicOn->isClicked())
	{
		audio->playMusicbyName("DungeonMusic");
		musicOn->reset();
	}
	if (musicOff->isClicked())
	{
		audio->stopAllMusic();
		musicOff->reset();
	}
}

void OptionsMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(*musicOn);
	window->draw(*musicOff);
	window->draw(*backButton);
	window->draw(titleText);
	window->draw(instructionText);
	endDraw();
}