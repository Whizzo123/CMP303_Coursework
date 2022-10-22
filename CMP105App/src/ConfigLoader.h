#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

struct GameWeapons
{
	std::string name;
	int damage;
	float speed;
};
struct GameArmour
{
	std::string name;
	int armourValue;
	int armourType;
};


class ConfigLoader
{
public:
	ConfigLoader();
	~ConfigLoader();
    static void loadWeaponsFromFile();
	static void loadArmoursFromFile();
	static std::vector<GameWeapons> getWeaponsLoaded();
	static std::vector<GameArmour> getArmoursLoaded();

private:
	//Vector for all loaded weapons
	static std::vector<GameWeapons> weapons;
	//Vector for all loaded armours
	static std::vector<GameArmour> armours;
};


