#ifndef ZONE_H
#define ZONE_H

#include <SFML\Graphics.hpp>
#include "ImageManager.h"
#include "Building.h"

class Level;
class Zone : public Building
{
friend class City;
protected:
public:
	Zone(unsigned int id, int x, int y, int z, bool preview);
	void Reset();
	sf::Vector2u GetDisplayTile(Level* level);
	sf::Vector2u GetFootprint();
};

#endif
