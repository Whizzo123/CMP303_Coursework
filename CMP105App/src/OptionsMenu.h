#pragma once
#include "Level.h"
#include "LevelLoaderButton.h"
class OptionsMenu : public Level
{
public:
	OptionsMenu() {}
	OptionsMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~OptionsMenu() {}
	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;
private:
	GameObject background;
	sf::Texture backgroundTexture;
	LevelLoaderButton* backButton;
	Button* musicOff;
	Button* musicOn;
	sf::Font titleFont;
	sf::Text titleText;
	sf::Text instructionText;
};

