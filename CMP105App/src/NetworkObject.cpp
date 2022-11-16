#include "NetworkObject.h"

NetworkObject::NetworkObject() : GameObject(),
	objectID(0), nposition(-1.0f, -1.0f)
{
	
}

NetworkObject::NetworkObject(int ID, bool clientUpdateable) : GameObject(),
	objectID(ID), nposition(-1.0f, -1.0f)
{
	canClientUpdate = clientUpdateable;
}

NetworkObject::~NetworkObject()
{
}

void NetworkObject::Update(float deltaSeconds)
{
	
}

void NetworkObject::Start()
{
}
