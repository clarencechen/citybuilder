#include <SFML\Graphics.hpp>
#include <math.h>
#include <iostream>
#include "Level.h"
#include "Building.h"
#include "ImageManager.h"
#include "Infrastructure.h"
#include "TerrainTile.h"

Building::Building(int x, int y, int z, bool preview)
{
	this->x = x;
	this->y = y;
	for(int i = 0; i < 4; i++)
		this->z[i] = z;
	this->preview = preview;
	this->buffer = sf::Vector2u(0, 0);
	this->condemn = false;
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
}

Building::~Building()
{
	//dtor
}

void Building::SetStatus(sf::Vector2u status, bool preview, ImageManager& imageManager)
{
	if(!preview)
	{
		buffer.x = status.x;
		buffer.y = status.y;
	}
	baseid = status.x;
	transform = status.y;
	texture = sf::Texture(imageManager.GetImage(baseid));
}

void Building::Reset(ImageManager& imageManager)
{
	preview = false;
	baseid = buffer.x;
	transform = buffer.y;
	texture = sf::Texture(imageManager.GetImage(baseid));
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

void Building::Draw(sf::Vector2i camOffset, sf::RenderWindow* rw)
{
	sf::VertexArray quad(sf::Quads, 4);
	quad[1].position = sf::Vector2f(y*tilesize		+x*tilesize		-camOffset.x,
									y*tilesize/2	-x*tilesize/2	-camOffset.y																	-z[1]*tilesize/4);
	quad[2].position = sf::Vector2f(y*tilesize		+x*tilesize		-camOffset.x	+(int)texture.getSize().y,
									y*tilesize/2	-x*tilesize/2	-camOffset.y	-(int)(texture.getSize().y)/2									-z[2]*tilesize/4);
	quad[3].position = sf::Vector2f(y*tilesize		+x*tilesize		-camOffset.x	-(int)texture.getSize().x		+(int)texture.getSize().y,
									y*tilesize/2	-x*tilesize/2	-camOffset.y	-((int)(texture.getSize().x)	+(int)(texture.getSize().y))/2	-z[3]*tilesize/4);
	quad[0].position = sf::Vector2f(y*tilesize		+x*tilesize		-camOffset.x	-(int)texture.getSize().x,
									y*tilesize/2	-x*tilesize/2	-camOffset.y	-(int)(texture.getSize().x)/2									-z[0]*tilesize/4);
	switch (transform)
	{
		case 0:
		{
			quad[0].texCoords = sf::Vector2f(0, 0);
			quad[1].texCoords = sf::Vector2f(0, texture.getSize().y);
			quad[2].texCoords = sf::Vector2f(texture.getSize().x, texture.getSize().y);
			quad[3].texCoords = sf::Vector2f(texture.getSize().x, 0);
			break;
		}
		case 1:
		{
			quad[0].texCoords = sf::Vector2f(0, texture.getSize().y);
			quad[1].texCoords = sf::Vector2f(texture.getSize().x, texture.getSize().y);
			quad[2].texCoords = sf::Vector2f(texture.getSize().x, 0);
			quad[3].texCoords = sf::Vector2f(0, 0);
			break;
		}
		case 2:
		{
			quad[0].texCoords = sf::Vector2f(texture.getSize().x, texture.getSize().y);
			quad[1].texCoords = sf::Vector2f(texture.getSize().x, 0);
			quad[2].texCoords = sf::Vector2f(0, 0);
			quad[3].texCoords = sf::Vector2f(0, texture.getSize().y);
			break;
		}
		case 3:
		{
			quad[0].texCoords = sf::Vector2f(texture.getSize().x, 0);
			quad[1].texCoords = sf::Vector2f(0, 0);
			quad[2].texCoords = sf::Vector2f(0, texture.getSize().y);
			quad[3].texCoords = sf::Vector2f(texture.getSize().x, texture.getSize().y);
			break;
		}
	};
	if(preview)
		for(int i = 0; i < 4; i++)
			quad[i].color = sf::Color(255, 255, 255, 128);
	if(condemn)
		for(int i = 0; i < 4; i++)
			quad[i].color = sf::Color(255, 0, 0, 128);
	rw->draw(quad, &texture);
}
