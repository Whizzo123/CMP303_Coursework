#include "DragController.h"

DragController::DragController()
{

}

DragController::DragController(Input* input, InventoryManager* inventoryManager, sf::RenderWindow* window)
{
	//Initialize variables
	this->input = input;
	this->inventoryManager = inventoryManager;
	this->window = window;
	dragging = false;
}

void DragController::update(float dt)
{
	//Grab mouse position with respect to view
	sf::Vector2i mousePos = sf::Vector2i(input->getMouseX() + window->getView().getCenter().x - window->getSize().x / 2,
		input->getMouseY() + window->getView().getCenter().y - window->getSize().y / 2);
	//If left mouse button down and not dragging
	if (input->isLeftMouseDown() && !dragging)
	{
		Inventory* inventory = inventoryManager->getInventory(mousePos);
		//If we have an inventory and its showing
		if (inventory != nullptr  && inventory->getShowing())
		{
			Slot* slot = inventory->getSlotMousedOver();
			//If we are over a slot
			if (slot != nullptr)
			{
				holdingItem = slot->getItem();
				//If the slot has an item begin drag
				if (holdingItem != nullptr)
				{
					dragging = true;
					beginDragInventory = inventory;
					beginDragSlot = slot;
				}
			}
		}
	}
	//If left mouse button is not down and was dragging
	if (input->isLeftMouseDown() == false && dragging)
	{
		Inventory* inventory = inventoryManager->getInventory(mousePos);
		//If have an inventory and is showing
		if (inventory != nullptr && inventory->getShowing())
		{
			Slot* slot = inventory->getSlotMousedOver();
			//If over slot and is can accept current item
			if (slot != nullptr && slot->isItemCompatable(holdingItem))
			{
				//If holdingItem is a weapon
				if (dynamic_cast<Weapon*>(holdingItem))
				{
					//Give slot holding item
					slot->setItem(new Weapon(*(dynamic_cast<Weapon*>(holdingItem))));
				}
				//If holding item is a potion
				if (dynamic_cast<Potion*>(holdingItem))
				{
					//Give slot holding item
					slot->setItem(new Potion(*(dynamic_cast<Potion*>(holdingItem))));
				}
				//If holdingItem is a piece of armour
				if (dynamic_cast<Armour*>(holdingItem))
				{
					//Give slot holding item
					slot->setItem(new Armour(*(dynamic_cast<Armour*>(holdingItem))));
				}
				InventorySyncData data;
				data.invID = inventoryManager->getIDForInventory(beginDragInventory);
				data.slotID = beginDragInventory->getSlotID(beginDragSlot);
				data.otherInvID = inventoryManager->getIDForInventory(inventory);
				data.otherInvSlotID = inventory->getSlotID(slot);
				data.playerID = NetworkingManager::GetMyConnectionIndex();
				std::cout << "Sending inventory sync data" << std::endl;
				NetworkingManager::SendInventorySyncData(data);
				//Clear slot item was from
				beginDragSlot->clearItem();
				beginDragInventory = nullptr;
				//Clear holding item
				holdingItem = nullptr;
				//End drag
				dragging = false;
			}
			else
			{
				//End drag
				dragging = false;
				//Clear holdingItem
				holdingItem = nullptr;
			}
		}
		else
		{
			//End drag
			dragging = false;
			//Clear holdingitem
			holdingItem = nullptr;
		}
	}
}