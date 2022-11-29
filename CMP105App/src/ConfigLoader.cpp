#include "ConfigLoader.h"

std::vector<GameWeapons> ConfigLoader::weapons;
std::vector<GameArmour> ConfigLoader::armours;

ConfigLoader::ConfigLoader()
{
	
}

ConfigLoader::~ConfigLoader()
{

}

void ConfigLoader::loadWeaponsFromFile()
{
	//Open stream to weapons file
	std::ifstream reader("items/Weapons.txt");
	std::string line;

	//Check whether it was correctly found
	if (!reader)
	{
		std::cout << "Error opening input file" << std::endl;
		return;
	}
	//Assign character for splitting up line
	std::string delimiter = "|";
	for (int i = 0; ! reader.eof(); i++)
	{
		//Get new line
		getline(reader, line);
		int pos = 0;
		int j = 0;
		GameWeapons weapon;
		//While we can still find our splitting character
		while ((pos = line.find(delimiter)) != std::string::npos)
		{
			//Substring at our new found character
			std::string token = line.substr(0, pos);
			//Assign info depending on what part of line we've reached
			switch (j)
			{
			case 0:
				weapon.name = token;
				break;
			case 1:
				weapon.damage = std::stoi(token);
				break;
			case 2:
				weapon.speed = std::stof(token);
				break;
			}
			//Erase part of line we just read
			line.erase(0, pos + delimiter.length());
			//Increment j to alter part of line
			j++;
		}
		//Add to weapons vector
		weapons.push_back(weapon);
	}
}
void ConfigLoader::loadArmoursFromFile()
{
	//Open stream to armour file
	std::ifstream reader("items/Armours.txt");
	std::string line;
	//Check whether it was correctly found
	if (!reader)
	{
		std::cout << "Error opening input file" << std::endl;
		return;
	}
	//Assign character for splitting up line
	std::string delimiter = "|";
	for (int i = 0; !reader.eof(); i++)
	{
		//Get new line
		getline(reader, line);
		int pos = 0;
		int j = 0;
		GameArmour armour;
		//While we can still found our splitting character
		while ((pos = line.find(delimiter)) != std::string::npos)
		{
			//Substring at our new found character
			std::string token = line.substr(0, pos);
			//Assign info depending on what part of line we've reached
			switch (j)
			{
			case 0:
				armour.name = token;
				break;
			case 1:
				armour.armourValue = std::stoi(token);
				break;
			case 2:
				armour.armourType = std::stoi(token);
				break;
			}
			//Erase part of line we just read
			line.erase(0, pos + delimiter.length());
			//Increment j to alter part of line 
			j++;
		}
		//Add to armours vector
		armours.push_back(armour);
	}
}

std::vector<GameWeapons> ConfigLoader::getWeaponsLoaded()
{
	return weapons;
}

int ConfigLoader::getItemID(std::string name)
{
	for (int i = 0; i < weapons.size(); i++)
	{
		if (weapons[i].name == name)
			return i;
	}
	for (int i = 0; i < armours.size(); i++)
	{
		if (armours[i].name == name)
			return i;
	}
	return -1;
}

std::vector<GameArmour> ConfigLoader::getArmoursLoaded()
{
	return armours;
}