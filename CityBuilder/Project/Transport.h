#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <SFML\Graphics.hpp>
#include "Building.h"

class Level;
class Draggable : public Building
{
friend class City;
protected:
	unsigned int roadState;
	unsigned int roadBuffer;
	unsigned int railState;
	unsigned int railBuffer;
	Building* neighbors[4];
public:
	Draggable(unsigned int id, int x, int y, int z[4], bool preview, Level* level);
	Draggable(unsigned int id, int x, int y, int z, bool preview, Level* level);
	void SetRoad(unsigned int r, bool preview);
	void SetRail(unsigned int r, bool preview);
	void Add(unsigned int id, bool preview);
	void MatchNetwork(bool preview, Level* level);
	sf::Vector2u GetDisplayTile(Level* level);
	sf::Vector2u GetFootprint();
	void Reset();
	unsigned int GetRoad();
	unsigned int GetRail();
};

#endif

