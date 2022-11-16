#include "Player.h"

Player::Player(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, Input* input, sf::RenderWindow* window, AudioManager* audio,
	float maxHealth) : Character(textureName, pos, size, speed, maxHealth, audio)
{
	//Initialize variables
	setInput(input);
	setCollisionBox(0 + 25, 0 + 10, size.x - 35, size.y - 25);
	setupAnimations();
	setWindow(window);
	setAlive(true);
	currentAttackTimer = 0.0f;
	characterName = "Toby";
	interactRect = sf::FloatRect(-size.x, -size.y, 3 * size.x, 3 * size.y);
	this->chestManager = chestManager;
	this->dungeonExit = dungeonExit;
	this->damageAudioName = "DwarfDamage";
};

Player::~Player(){}

void Player::handleInput(float dt)
{
	if (input == nullptr)
	{
		return;
	}
	//Set velocity to 0
	velocity = sf::Vector2f();
	//Check for input on WASD keys
	if (input->isKeyDown(sf::Keyboard::W))
	{
		velocity.y -= speed * dt;
	}
	if (input->isKeyDown(sf::Keyboard::S))
	{
		velocity.y += speed * dt;
	}
	if (input->isKeyDown(sf::Keyboard::A))
	{
		velocity.x -= speed * dt;
		walkAnimation.setFlipped(true);
		idleAnimation.setFlipped(true);
	}
	if (input->isKeyDown(sf::Keyboard::D))
	{
		velocity.x += speed * dt;
		walkAnimation.setFlipped(false);
		idleAnimation.setFlipped(false);
	}
	//Check for input on space to attack as long as attack cooldown is up
	if (input->isPressed(sf::Keyboard::Space) && currentAttackTimer <= 0)
	{
		//Play attack animation
		attackAnimation.reset();
		if (lastDirection.x < 0)
			attackAnimation.setFlipped(true);
		else
			attackAnimation.setFlipped(false);
		attack();
		currentAnimation = &attackAnimation;
		currentAnimation->play(true);
		//Grab damage value from current equipped weapon
		currentAttackTimer = dynamic_cast<Weapon*>(inventory->getSlot(10)->getItem())->getSpeed();
	}
	//Check for input on E in order to interact
	if (input->isPressed(sf::Keyboard::E))
	{
		//Check for collision with dungoen exit if so transition to next level
		if (Collision::checkBoundingBox(this, dungeonExit))
		{
			audio->playSoundbyName("DungeonDoor");
			dungeonExit->loadLevel();
			return;
		}
		//Deal with inventory and chest interaction
		inventory->setShowing(!inventory->getShowing());
		sf::FloatRect* fr = getInteractRect();
		chestManager->checkToOpen(*getInteractRect());
	}
	//If velocity is not 0 update last direction
	if (velocity.x != 0 || velocity.y != 0)
	{
		lastDirection = velocity;
	}
	//If velocity is 0 update moving
	if (velocity.x == 0 && velocity.y == 0)
		moving = false;
	else
		moving = true;
	
}

void Player::update(float dt)
{	
	Character::update(dt);
	updateAnimations(dt);
	//If walking and not playing walk sound
	if (moving && audio->getSound("DwarfWalk")->getStatus() == sf::Sound::Status::Stopped)
	{
		//Play walk sound
		audio->playSoundbyName("DwarfWalk");
	}
	//If not walking and playing walk sound
	if(!moving && audio->getSound("DwarfWalk")->getStatus() == sf::Sound::Status::Playing)
	{
		//Stop walk sound
		audio->getSound("DwarfWalk")->stop();
	}
	//Move player
	move(velocity);
	//Use last direction to alter attack rect
	if (lastDirection.x < 0)
		attackRect = sf::FloatRect(getCollisionBox().left - getSize().x, getCollisionBox().top - getSize().y, getSize().x * 1.25, getSize().y * 3);
	else
		attackRect = sf::FloatRect(getCollisionBox().left + (getSize().x / 3.5), getCollisionBox().top - getSize().y, getSize().x * 1.25, getSize().y * 3);
	//Decrement attack cooldown if > 0
	if (currentAttackTimer > 0)
		currentAttackTimer -= dt;
}

void Player::collisionResponse(GameObject* other)
{
	Character::collisionResponse(other);
}

//Setup player animations
void Player::setupAnimations()
{
	for (int i = 0; i < 4; i++)
	{
		walkAnimation.addFrame(sf::IntRect(32 * i, 0, 32, 32));
	}
	walkAnimation.setFrameSpeed(0.2f);

	for (int i = 0; i < 4; i++)
	{
		idleAnimation.addFrame(sf::IntRect(32 * i, 32, 32, 32));
	}
	idleAnimation.setFrameSpeed(0.25f);

	for (int i = 0; i < 7; i++)
	{
		attackAnimation.addFrame(sf::IntRect(32 * i, 64, 32, 32));
	}
	attackAnimation.setLooping(false);
	attackAnimation.setFrameSpeed(0.2f);

	for (int i = 0; i < 4; i++)
	{
		damageAnimation.addFrame(sf::IntRect(32 * i, 96, 32, 32));
	}
	damageAnimation.setLooping(false);
	damageAnimation.setFrameSpeed(0.25f);

	for (int i = 0; i < 7; i++)
	{
		deathAnimation.addFrame(sf::IntRect(32 * i, 128, 32, 32));
	}
	deathAnimation.setLooping(false);
	deathAnimation.setFrameSpeed(0.2f);
	currentAnimation = &idleAnimation;
}

void Player::updateAnimations(float dt)
{
	Character::updateAnimations(dt);
}

void Player::drawDebugInfo()
{
	attackRectDebug.setSize(sf::Vector2f(attackRect.width, attackRect.height));
	attackRectDebug.setPosition(sf::Vector2f(attackRect.left, attackRect.top));
	attackRectDebug.setOutlineColor(sf::Color::Magenta);
}

void Player::attack()
{
	//If has enemyTarget
	if (enemyTarget != nullptr)
	{
		//Damage target using weapon stats
		enemyTarget->damage(dynamic_cast<Weapon*>(inventory->getSlot(10)->getItem())->getDamage(), getPosition());
		//Play attack sound
		audio->playSoundbyName("DwarfAttack");
	}
}

void Player::setEnemyTarget(Character* other)
{
	enemyTarget = other;
}

sf::FloatRect* Player::getAttackRect()
{
	return &attackRect;
}

sf::FloatRect* Player::getInteractRect()
{
	return new sf::FloatRect(interactRect.left + getPosition().x, interactRect.top + getPosition().y, interactRect.width, interactRect.height);
}

void Player::onDeath()
{
	debugLog("Player Dead");
	*nextLevel = 8;
}

ChestManager* Player::getChestManager()
{
	return chestManager;
}

void Player::setChestManager(ChestManager* chestManager)
{
	this->chestManager = chestManager;
}

DungeonExit* Player::getDungeonExit()
{
	return dungeonExit;
}

void Player::setDungeonExit(DungeonExit* dungeonExit)
{
	this->dungeonExit = dungeonExit;
}

Inventory* Player::getInventory()
{
	return inventory;
}

int* Player::getNextLevel()
{
	return nextLevel;
}

void Player::setNextLevel(int* nextLevel)
{
	this->nextLevel = nextLevel;
}

