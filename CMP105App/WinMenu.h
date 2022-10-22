#pragma once
#include "Level.h"
class WinMenu : public Level
{
public:
	WinMenu() {}
	WinMenu(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~WinMenu() {}
	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;
private:
	GameObject background;
	sf::Texture backgroundTexture;
	sf::Font titleFont;
	sf::Text titleText;
};

