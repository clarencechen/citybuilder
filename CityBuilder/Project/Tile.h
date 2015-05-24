#ifndef _TERRAINTILE_H
#define _TERRAINTILE_H

#include <SFML\Graphics.hpp>
class TerrainTile
{
private:
	int x;
	int y;
	int z[4];
    bool preview[4];
    sf::Texture* texture;
public:
	static const int tilesize = 32;
	TerrainTile(int x, int y, sf::Texture* tex);
	~TerrainTile();
	void Draw(sf::Vector2i camOffset, sf::RenderWindow* rw);
	int(&GetHeight())[4];
	void Raise(int elevation, int corner);
	void SetHeight(int elevation, int corner);
	void Reset();
};

#endif
