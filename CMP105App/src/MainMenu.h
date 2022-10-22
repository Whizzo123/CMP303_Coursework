#pragma once
#include "Level.h"
#include "LevelLoaderButton.h"
#include "NetworkButton.h"
class MainMenu : public Level
{
public:
	MainMenu() {}
	MainMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~MainMenu() {}
	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;
private:
	GameObject background;
	sf::Texture backgroundTexture;
	LevelLoaderButton* playButton;
	NetworkButton* hostButton;
	NetworkButton* joinButton;
	LevelLoaderButton* optionsButton;
	LevelLoaderButton* creditsButton;
	sf::Font titleFont;
	sf::Text titleText;
};
