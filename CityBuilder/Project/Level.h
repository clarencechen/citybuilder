#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include "Tile.h"
#include "ImageManager.h"
class Building;
class Draggable;
class Zone;
#include "Building.h"
class Level
{
private:
    sf::Clock clock;
	sf::Time t;
	sf::Texture* tex;
	//A 2D array of Tile pointers
	std::vector<std::vector<TerrainTile*> > map;
	//A 3D array of Building Pointers
	std::vector<std::vector<std::vector<Draggable*> > > bridges;
	std::vector<std::vector<Building*> > buildings;
	std::vector<int> shuffledInts;
	//Width and height of level (in tiles)
	int w;
	int h;
	sf::Vector2u footprint[128] = {
	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),
	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),
	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),
	sf::Vector2u(2,2),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),
	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),
	};
	void SetDimensions(int w, int h);

public:
	Level();
	~Level();
	void Reset();
	void Place(int x, int y, int z, bool stick, unsigned int r, bool preview);
	void Place(int x, int y, int z, unsigned int r, bool preview);

	Building*(&GetNeighbors(unsigned int x, unsigned int y, int z, bool stick))[4];

	void Terraform(int x, int y, int raise);
	int GetHeight(int x, int y, int corner);
	int GradeBuilding(sf::IntRect bounds, bool preview);

	std::vector<Draggable*> GetBridge(unsigned int x, unsigned int y);
	Draggable* GetBridge(unsigned int x, unsigned int y, int z);
	Building* GetBuilding(unsigned int x, unsigned int y);
	void Shuffle();
	unsigned int GetShuffled(unsigned int i);

	TerrainTile* GetTile(unsigned int x, unsigned int y);

	void LoadLevel(std::string filename, ImageManager& imageManager);

	int GetWidth();
	int GetHeight();
};

#endif
