#ifndef BUILDING_H
#define BUILDING_H

#include <SFML\Graphics.hpp>
#include "Level.h"
#include "ImageManager.h"

class Level;
class Building
{
protected:
	int x;
	int y;
	int z[4];
	sf::Vector2u buffer;
	unsigned int baseid;
    unsigned int transform;
    bool preview;
    bool condemn;
	sf::Texture texture;
public:
	static const int tilesize = 32;
	Building(int x, int y, int z, bool preview);
	Building(int x, int y, int z[4], bool preview);
	~Building();
	void Draw(sf::Vector2i camOffset, sf::RenderWindow* rw);
	virtual void Reset(ImageManager& imageManager);
	void SetStatus(sf::Vector2u status, bool preview, ImageManager& imageManager);
	bool GetDel();
	bool GetPreview();
	void Condemn();
	virtual unsigned int GetRoad() = 0;
	virtual unsigned int GetRail() = 0;
	virtual void Add(unsigned int id, bool preview) = 0;
	virtual sf::Vector2u GetFootprint() = 0;
	virtual sf::Vector2u GetDisplayTile(Level* level) = 0;
	virtual void MatchNetwork(bool preview, Level* level) = 0;
};
#endif // BUILDING_H
