#pragma once
#include "Framework/GameObject.h"

class NetworkObject : public GameObject
{

public:
	NetworkObject();
	NetworkObject(int ID, bool clientUpdateable);
	~NetworkObject();

	virtual void Update(float deltaSeconds);
	virtual void Start();
	
	int GetID() { return objectID; }
	bool canUpdate() { return canClientUpdate; }

protected:
	sf::Vector2f nposition;
	int objectID;
	/// <summary>
	/// If true client can send updates to server for this object
	/// </summary>
	bool canClientUpdate;
};

