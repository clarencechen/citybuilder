#include <SFML\Graphics.hpp>
#include <math.h>
#include <iostream>
#include "Level.h"
#include "Building.h"
#include "Infrastructure.h"
#include "Tile.h"

Building::Building(int x, int y, int z, bool preview)
{
	this->x = x;
	this->y = y;
	for(int i = 0; i < 4; i++)
		this->z[i] = z;
	this->preview = preview;
	this->buffer = sf::Vector2u(0, 0);
	this->condemn = false;
	stick = false;
	texture = 0;
	variant = 0;
    residents = 0;
    maxPopPerVariant = 0;
    maxVariants = 0;
    production = 0;
    storedGoods = 0;
}

Building::Building(int x, int y, int z[4], bool preview)
{
	this->x = x;
	this->y = y;
	for(int i = 0; i < 4; i++)
		this->z[i] = z[i];
	this->preview = preview;
	this->buffer = sf::Vector2u(0, 0);
	this->condemn = false;
	stick = true;
	variant = 0;
    residents = 0;
    maxPopPerVariant = 0;
    maxVariants = 0;
    production = 0;
    storedGoods = 0;
}

Building::~Building()
{

}

void Building::SetStatus(sf::Vector2u status, bool preview)
{
	if(!preview)
	{
		buffer.x = status.x;
		buffer.y = status.y;
	}
	baseid = status.x;
	transform = status.y;
}

void Building::Reset()
{
	preview = false;
	baseid = buffer.x;
	transform = buffer.y;
	condemn = false;
}

bool Building::GetPreview()
{
	return preview;
}
bool Building::GetDel()
{
	return buffer == sf::Vector2u(0, 0);
}
void Building::Condemn()
{
	condemn = true;
}
sf::Vector3f Building::GetPop()
{
    return sf::Vector3f(residents, storedGoods, production);
}
unsigned int Building::GetId()
{
    return id;
}
sf::Vector2u Building::GetAnchor()
{
    return sf::Vector2u (x, y);
}
bool Building::operator==(const Building &other)
{
    return(this->x == other.x && this->y == other.y && this->stick == other.stick);
}
unsigned int Building::GetRoad() {return 0;}
unsigned int Building::GetRail() {return 0;}
void Building::Add(unsigned int id, bool preview) {}
void Building::MatchNetwork(bool preview, Level* level) {}

void Building::Draw(sf::RenderWindow* rw, ImageManager& imageManager)
{
	texture = imageManager.GetImage(baseid);
	sf::VertexArray quad(sf::Quads, 4);
	quad[1].position = sf::Vector2f(y*tilesize		+x*tilesize		,
									y*tilesize/2	-x*tilesize/2																		-z[1]*tilesize/4);
	quad[2].position = sf::Vector2f(y*tilesize		+x*tilesize		+(int)(texture->getSize().y),
									y*tilesize/2	-x*tilesize/2	-(int)(texture->getSize().y)/2									-z[2]*tilesize/4);
	quad[3].position = sf::Vector2f(y*tilesize		+x*tilesize		-(int)(texture->getSize().x)	+(int)(texture->getSize().y),
									y*tilesize/2	-x*tilesize/2	-((int)(texture->getSize().x)	+(int)(texture->getSize().y))/2	-z[3]*tilesize/4);
	quad[0].position = sf::Vector2f(y*tilesize		+x*tilesize		-(int)(texture->getSize().x),
									y*tilesize/2	-x*tilesize/2	-(int)(texture->getSize().x)/2									-z[0]*tilesize/4);
	switch (transform)
	{
		case 0:
		{
			quad[0].texCoords = sf::Vector2f(0, 0);
			quad[1].texCoords = sf::Vector2f(0, texture->getSize().y);
			quad[2].texCoords = sf::Vector2f(texture->getSize().x, texture->getSize().y);
			quad[3].texCoords = sf::Vector2f(texture->getSize().x, 0);
			break;
		}
		case 1:
		{
			quad[0].texCoords = sf::Vector2f(0, texture->getSize().y);
			quad[1].texCoords = sf::Vector2f(texture->getSize().x, texture->getSize().y);
			quad[2].texCoords = sf::Vector2f(texture->getSize().x, 0);
			quad[3].texCoords = sf::Vector2f(0, 0);
			break;
		}
		case 2:
		{
			quad[0].texCoords = sf::Vector2f(texture->getSize().x, texture->getSize().y);
			quad[1].texCoords = sf::Vector2f(texture->getSize().x, 0);
			quad[2].texCoords = sf::Vector2f(0, 0);
			quad[3].texCoords = sf::Vector2f(0, texture->getSize().y);
			break;
		}
		case 3:
		{
			quad[0].texCoords = sf::Vector2f(texture->getSize().x, 0);
			quad[1].texCoords = sf::Vector2f(0, 0);
			quad[2].texCoords = sf::Vector2f(0, texture->getSize().y);
			quad[3].texCoords = sf::Vector2f(texture->getSize().x, texture->getSize().y);
			break;
		}
	};
	if(preview)
		for(int i = 0; i < 4; i++)
			quad[i].color = sf::Color(255, 255, 255, 192);
	if(condemn)
		for(int i = 0; i < 4; i++)
			quad[i].color = sf::Color(255, 0, 0, 128);
    sf::RenderStates rs(texture);
	rw->draw(quad, rs);
}
