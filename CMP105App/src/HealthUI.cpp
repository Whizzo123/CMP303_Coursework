#include "HealthUI.h"

HealthUI::HealthUI(Player* player, sf::RenderWindow* window, sf::Vector2f pos)
{
	//Initialize variables
	this->player = player;
	this->window = window;
	font.loadFromFile("font/arial.ttf");
	text.setFont(font);
	originPos = pos;
}

void HealthUI::update()
{
	//Update health shown
	text.setPosition(originPos + sf::Vector2f(window->getView().getCenter().x - window->getSize().x / 2, window->getView().getCenter().y - window->getSize().y / 2));
	std::stringstream ss;
	std::string healthText;
	ss << std::setprecision(2) << "HP: " << player->getCurrentHealth() << "/" << player->getMaxHealth();
	text.setString(ss.str());
}

void HealthUI::draw()
{
	window->draw(text);
}