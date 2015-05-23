#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <SFML\Graphics.hpp>
#include "ImageManager.h"

class Level;
class Structure : public Building
{
protected:
	unsigned int idbuffer;
public:
	unsigned int id;
	Structure(unsigned int id, int x, int y, int z, bool preview);
	void Reset(ImageManager& imageManager);
	//below 5 used for draggables only
	void Add(unsigned int id, bool preview);
	sf::Vector2u GetDisplayTile(Level* level);
	void MatchNetwork(bool preview, Level* level);
	unsigned int GetRoad();
	unsigned int GetRail();

	sf::Vector2u GetFootprint();
};

#endif
