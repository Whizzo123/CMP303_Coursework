#pragma once
#include "Level.h"
#include "LevelLoaderButton.h"
class DeathMenu : public Level
{
public:
	DeathMenu() {}
	DeathMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~DeathMenu() {}
	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;
private:
	GameObject background;
	sf::Texture backgroundTexture;
	LevelLoaderButton* quitButton;
	sf::Font titleFont;
	sf::Text titleText;
};


