#pragma once
#include "Framework/GameObject.h"
#include "Player.h"
#include <math.h>
#include <iostream>
using namespace std;

class Background : public GameObject
{

public:
	Background(sf::RenderWindow *gameWindow, Player* player);
	Background();
	~Background() {};
	void update(float dt) override;

private:
	//The window view
	sf::View view; 
	//Speed of camera movement
	float scrollSpeed;
	//Reference to player
	Player* player;
	//How far player should move before we start following
	float moveThreshold;
};

