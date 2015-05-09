#ifndef BUILDING_H
#define BUILDING_H

#include <SFML\Graphics.hpp>
#include "ImageManager.h"

class Building
{
protected:
	int x;
	int y;
	int z[4];
	sf::Vector2u buffer;
	unsigned int idbuffer;
	unsigned int baseid;
    unsigned int transform;
    bool preview;
	sf::Texture texture;
public:
	unsigned int id;
	static const int tilesize = 32;
	Building(int id, int x, int y, int z, bool preview);
	Building(int id, int x, int y, int z[4], bool preview);
	~Building();
	void Draw(sf::Vector2i camOffset, sf::RenderWindow* rw);
	void SetStatus(ImageManager& imageManager);
	void SetStatus(unsigned int r, bool preview);
	void SetStatus(sf::Vector2u status, bool preview, ImageManager& imageManager);
	bool GetDel();
	bool GetPreview();
};
#endif // BUILDING_H
