#ifndef _LEVEL_H
#define _LEVEL_H

#include <string>
#include <vector>
#include "Tile.h"
#include "ImageManager.h"
#include "Building.h"

class Level
{
private:
	//A 2D array of Tile pointers
	std::vector<std::vector<TerrainTile*> > map;
	//A 3D array of Building Pointers
	std::vector<std::vector<std::vector<Building*> > > bridges;
	std::vector<std::vector<Building*> > buildings;
	//Width and height of level (in tiles)
	int w;
	int h;
	sf::Vector2u footprint[128] = {
	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),
	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),
	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),
	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),	sf::Vector2u(1,1),
	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),	sf::Vector2u(2,2),
	};
	void SetDimensions(int w, int h);

public:
	Level();
	~Level();
	void Reset(ImageManager& imageManager);
	void Place(int x, int y, int z, bool stick, unsigned int r, bool preview, ImageManager& ImageManager);
	void Place(int x, int y, int z, unsigned int r, bool preview, ImageManager& imageManager);

	void MatchRoad(int x, int y, int z, bool stick);
	int(&GetNeighbors(unsigned int x, unsigned int y, int z, bool stick))[4];
	sf::Vector2u GetBuildingTile(unsigned int x, unsigned int y, int z, bool stick);

	void Terraform(int x, int y, int raise);
	int GetHeightForMouse(int x, int y);
	void GradeBuilding(sf::IntRect bounds);

	void AddBuilding(unsigned int r, int x, int y, int z, bool preview);
	void AddRoad(unsigned int r, int x, int y, int z, bool preview);

	std::vector<Building*> GetBridge(unsigned int x, unsigned int y);
	Building* GetBridge(unsigned int x, unsigned int y, int z);
	unsigned int GetId(unsigned int x, unsigned int y, int z, bool stick);
	Building* GetBuilding(unsigned int x, unsigned int y);

	void AddTile(int x, int y, TerrainTile* tile);
	TerrainTile* GetTile(unsigned int x, unsigned int y);

	void LoadLevel(std::string filename, ImageManager& imageManager);

	int GetWidth();
	int GetHeight();
};

#endif
