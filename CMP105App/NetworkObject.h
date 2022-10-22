#pragma once
#include "Framework/GameObject.h"
class NetworkObject : public GameObject
{

public:
	NetworkObject();
	~NetworkObject();

	virtual void Update(float deltaSeconds);
	virtual void Start();
	

protected:
	sf::Vector2f position;
};

