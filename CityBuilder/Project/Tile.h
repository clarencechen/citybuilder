#ifndef _TILE_H
#define _TILE_H

#include <SFML\Graphics.hpp>
#include "ImageManager.h"
class TerrainTile
{
private:
	int x;
	int y;
	int z[4];
	unsigned int baseid;
    unsigned int transform;
    bool preview[4];
    sf::Texture texture;
public:
	static const int tilesize = 32;
	TerrainTile(int baseid, int x, int y, int transform, ImageManager& imageManager);
	~TerrainTile();
	void Draw(sf::Vector2i camOffset, sf::RenderWindow* rw);
	void SetStatus();
	int(&GetHeight())[4];
	void Raise(int elevation, int corner);
	void SetHeight(int elevation, int corner);
	void Reset();
};

#endif
