#include "Character.h"

Character::Character(std::string textureName, sf::Vector2f pos, sf::Vector2f size, float speed, int maxHealth, AudioManager* audio)
{
	//Load character texture from path
	std::string textureNameToLoad = "gfx/" + textureName + ".png";
	texture.loadFromFile(textureNameToLoad);
	//Load damage text font from path
	font.loadFromFile("font/arial.ttf");
	statsIndicator.setFont(font);
	//Initialize gameobject variables
	setTexture(&texture);
	setPosition(pos);
	setSize(size);
	setCollisionBox(0, 0, size.x, size.y);
	//Initialize character variables
	this->speed = speed;
	moving = false;
	this->maxHealth = maxHealth;
	currentHealth = maxHealth;
	this->audio = audio;
	alive = false;
	dead = false;
	showingStatsIndicator = false;
	statShowingTimer = 0.5f;
	currentStatShowingTimer = 0.5f;
	characterName = "Default";
	//debugLog("Hiya!!!");
}

void Character::update(float dt)
{
	//If displaying damage text and timer is below 0
	if (showingStatsIndicator && currentStatShowingTimer < 0)
	{
		//Stop showing damage text
		showingStatsIndicator = false;
		currentStatShowingTimer = statShowingTimer;
	}
	else if (showingStatsIndicator)
	{
		//Move damage text up
		statsIndicator.setPosition(statsIndicator.getPosition() + (sf::Vector2f(0, -50) * dt));
		//Decrement timer
		currentStatShowingTimer -= dt;
	}
		
}

void Character::collisionResponse(GameObject* other)
{
	//Grab character and other objects collision boxes
	sf::FloatRect characterBox = getCollisionBox();
	sf::FloatRect otherBox = other->getCollisionBox();
	sf::Vector2f centerCharacter = sf::Vector2f(characterBox.left + (characterBox.width / 2), characterBox.top + (characterBox.height / 2));
	//If character is to the right of the object
	if ((characterBox.left < otherBox.left + otherBox.width && characterBox.left > otherBox.left) && (centerCharacter.y > otherBox.top && centerCharacter.y < otherBox.top + otherBox.height))
	{
		if (velocity.x < 0)
			velocity.x = 0;
	}
	//If the character is to the right of the object
	if ((characterBox.left + characterBox.width > otherBox.left && characterBox.left + characterBox.width < otherBox.left + otherBox.width) && (centerCharacter.y > otherBox.top && centerCharacter.y < otherBox.top + otherBox.height))
	{
		if (velocity.x > 0)
			velocity.x = 0;
	}
	//If the character is below the object
	if ((characterBox.top < otherBox.top + otherBox.height && characterBox.top > otherBox.top) && (centerCharacter.x > otherBox.left && centerCharacter.x < otherBox.left + otherBox.width))
	{
		if (velocity.y < 0)
			velocity.y = 0;
	}
	//If the character is above the object
	if ((characterBox.top + characterBox.height > otherBox.top && characterBox.top + characterBox.height < otherBox.top + otherBox.height) &&
		(centerCharacter.x > otherBox.left && centerCharacter.x < otherBox.left + otherBox.width))
	{
		if (velocity.y > 0)
			velocity.y = 0;
	}
}

void Character::updateAnimations(float dt)
{
	//If character is dead and not playing death animation or is playing death animation but has finished
	if ((dead && currentAnimation != &deathAnimation) || (currentAnimation == &deathAnimation && currentAnimation->getPlaying() == false))
	{
		//Remove character
		onDeath();
		alive = false;
	}
	//If playing damage animation and has finished
	if (currentAnimation == &damageAnimation && currentAnimation->getPlaying() == false)
	{
		//If character is moving
		if (moving)
			currentAnimation = &walkAnimation;
		else
			currentAnimation = &idleAnimation;
	}
	//If playing attack animation and has finished
	if (currentAnimation == &attackAnimation && currentAnimation->getPlaying() == false)
	{
		//If character is moving
		if (moving)
			currentAnimation = &walkAnimation;
		else
			currentAnimation = &idleAnimation;
	}
	//If not moving and playing walk animation
	else if (!moving && currentAnimation == &walkAnimation)
	{
		//Transition to idle animation
		currentAnimation = &idleAnimation;
	}
	//If not moving and playing idle animation
	else if (moving && currentAnimation == &idleAnimation)
	{
		//Transition to walk animation
		currentAnimation = &walkAnimation;
	}
	if (currentAnimation->getSize() == 0)
		currentAnimation = &idleAnimation;
	//Play current animation
	currentAnimation->animate(dt);
	//Set character texture to current frame of animation
	setTextureRect(currentAnimation->getCurrentFrame());

}


void Character::heal(int healPoints)
{
	//Set up stat text
	statsIndicator.setString(std::to_string(healPoints));
	statsIndicator.setFillColor(sf::Color::Green);
	showingStatsIndicator = true;
	//Heal player
	currentHealth += healPoints;
	//Ensure player can't heal past max health
	if (currentHealth > maxHealth)
		currentHealth = maxHealth;
}

void Character::damage(double damage, sf::Vector2f enemyPos)
{
	//If character is alive
	if (!dead)
	{
		//If has an inventory take armour into account
		if (inventory != nullptr)
			damage -= ((double)inventory->grabArmourValue() / 10);
		//Setup damage text
		std::stringstream ss;
		ss << std::setprecision(2) << damage;
		std::string damageText;
		ss >> damageText;
		statsIndicator.setString(damageText);
		statsIndicator.setFillColor(sf::Color::Red);
		showingStatsIndicator = true;
		//Deal damage and calculate knockback
		currentHealth -= damage;
		//calculateKnockback(enemyPos);
		//Play specific damage sound for this character
		audio->playSoundbyName(damageAudioName);
		//Check whether character has been killed by that damage
		if (currentHealth <= 0)
		{
			currentAnimation = &deathAnimation;
			dead = true;
		}
		else
		{
			//Play damage animation
			damageAnimation.reset();
			if (lastDirection.x < 0)
				damageAnimation.setFlipped(true);
			else
				damageAnimation.setFlipped(false);
			currentAnimation = &damageAnimation;
			currentAnimation->play(true);
		}
	}
}

void Character::debugLog(std::string msg)
{
	std::cout << characterName << " says " << msg << std::endl;
}

void Character::drawStatIndicator(sf::RenderWindow* window)
{
	//If showing stats text draw to screen
	if (showingStatsIndicator)
	{
		statsIndicator.setPosition(getPosition().x, getPosition().y);
		window->draw(statsIndicator);
	}
}

void Character::setInventory(Inventory* inventory)
{
	this->inventory = inventory;
}

void Character::calculateKnockback(sf::Vector2f enemyPos)
{
	//Get attack direction and use for knockback
	sf::Vector2f knockbackDirection = VectorHelper::normalise(getPosition() - enemyPos);
	float knockbackPower = 50.0f;
	velocity = (knockbackDirection * knockbackPower);
}

float Character::getCurrentHealth()
{
	return currentHealth;
}

float Character::getMaxHealth()
{
	return maxHealth;
}

void Character::resetHealth()
{
	currentHealth = maxHealth;
	alive = true;
	dead = false;
}