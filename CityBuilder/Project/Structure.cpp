#include <SFML\Graphics.hpp>
#include <math.h>
#include "Building.h"
#include "Structure.h"
unsigned int constexpr Structure::maxPopRef[16];
Structure::Structure(unsigned int id, int x, int y, int z, bool preview) : Building(x, y, z, preview)
{
	type = Type::STRUCTURE;
	this->id = id;
	idbuffer = preview ? 0 : id;
	maxPopServed = Structure::maxPopRef[id -32];
}

void Structure::Reset()
{
	Building::Reset();
	id = idbuffer;
}

sf::Vector2u Structure::GetDisplayTile(Level* level)
{
	return sf::Vector2u(id + 96, 0);
}

sf::Vector2u Structure::GetFootprint()
{
	if(id == 44)
		return sf::Vector2u(2, 3);
	return sf::Vector2u(2, 2);
}
