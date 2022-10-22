#include "Background.h"

Background::Background()
{

}

Background::Background(sf::RenderWindow *gameWindow, Player* player)
{
	setWindow(gameWindow);
	//Set view to windows view
	view = window->getView();
	scrollSpeed = 500.0f;
	this->player = player;
	moveThreshold = 2.0f;
}

void Background::update(float dt)
{
	//If the distance between the player and the center of the screen is greater than the threshold
	if (std::abs(player->getPosition().x - view.getCenter().x) > moveThreshold || std::abs(player->getPosition().y - view.getCenter().y) > moveThreshold)
	{
		//Move view in direction of player
		view.move((player->getPosition() - view.getCenter()) * dt);
		//Set window view to our view
		window->setView(view);
	}
	//If the distance between the player and center is under threshold but greater than zero
	else if (std::abs(player->getPosition().x - view.getCenter().x) > 0 || std::abs(player->getPosition().y - view.getCenter().y) > 0)
	{
		//Set view to player position
		view.setCenter(player->getPosition());
		//Set window view to our view
		window->setView(view);
	}
}