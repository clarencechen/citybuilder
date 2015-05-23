#include <SFML\Graphics.hpp>
#include <math.h>
#include "Level.h"
#include "Building.h"
#include "ImageManager.h"
#include "Infrastructure.h"
#include "TerrainTile.h"
#include "Structure.h"

Structure::Structure(unsigned int id, int x, int y, int z, bool preview) : Building(x, y, z, preview)
{
	this->id = id;
	idbuffer = preview ? 0 : id;
}

void Structure::Reset(ImageManager& imageManager)
{
	Building::Reset(imageManager);
	id = idbuffer;
}

void Structure::Add(unsigned int id, bool preview) {}
void Structure::MatchNetwork(bool preview, Level* level) {}
sf::Vector2u Structure::GetDisplayTile(Level* level)
{
	return sf::Vector2u(id + 96, 0);
}

sf::Vector2u Structure::GetFootprint()
{
	return sf::Vector2u(2, 2);
}
unsigned int Structure::GetRoad() {return 0;};
unsigned int Structure::GetRail() {return 0;};
