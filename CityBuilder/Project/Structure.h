#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <SFML\Graphics.hpp>
#include "Building.h"

class Level;
class Structure : public Building
{
friend class City;
protected:
	unsigned int maxPopServed;
public:
	static unsigned int constexpr maxPopRef[16] = {1000, 500, 500, 500, 1000, 1000, 0, 0, 0, 0, 0, 0, 1000};
	Structure(unsigned int id, int x, int y, int z, bool preview);
	void Reset();
	sf::Vector2u GetDisplayTile(Level* level);
	sf::Vector2u GetFootprint();
};

#endif
