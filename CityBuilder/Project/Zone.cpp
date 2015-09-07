#include <SFML\Graphics.hpp>
#include <math.h>
#include "Level.h"
#include "Building.h"
#include "Infrastructure.h"
#include "Tile.h"
#include "Structure.h"

Zone::Zone(unsigned int id, int x, int y, int z, bool preview) : Building(x, y, z, preview)
{
	type = Type::ZONE;
    maxPopPerVariant = 5;
/*	if(r >= 16 && r <= 18)
        maxVariants = 3;
    else if(r >= 19 && r <= 21)
        maxVariants = 6;
    else if(r >= 22 && r <= 24)
        maxVariants = 9;*/
	maxVariants = 4;
	this->id = id;
	idbuffer = preview ? 0 : id;
}

void Zone::Reset()
{
	Building::Reset();
	id = idbuffer;
}

sf::Vector2u Zone::GetDisplayTile(Level* level)
{
    //return sf::Vector2u(id +96 +variant*80, 0);
    return sf::Vector2u(id +96 +variant*20, 0);
}

sf::Vector2u Zone::GetFootprint()
{
	return sf::Vector2u(2, 2);
}
