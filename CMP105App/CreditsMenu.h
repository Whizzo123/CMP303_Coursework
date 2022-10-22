#pragma once
#include "Level.h"
#include "LevelLoaderButton.h"
class CreditsMenu : public Level
{
public:
	CreditsMenu() {}
	CreditsMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~CreditsMenu() {}
	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;
private:
	GameObject background;
	sf::Texture backgroundTexture;
	LevelLoaderButton* backButton;
	sf::Font titleFont;
	sf::Text titleText;
	sf::Text creditsText;

};

