

#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	//Initialize variables
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;
	nextLevel = -1;
}

Level::~Level()
{

}

int Level::getNextLevel()
{
	return nextLevel;
}

// handle user input
void Level::handleInput(float dt)
{

}

// Update game objects
void Level::update(float dt)
{
	
}

// Render level
void Level::render()
{
	beginDraw();
	endDraw();
}

void Level::resetNextLevel()
{
	nextLevel = -1;
}
