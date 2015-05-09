#include "ImageManager.h"
#include "Infrastructure.h"
#include "Tile.h"
#include <SFML\Graphics.hpp>
#include <math.h>
#include <iostream>

TerrainTile::TerrainTile(int baseid, int x, int y, int transform, ImageManager& imageManager)
{
	this->x = x;
	this->y = y;
	for(int i = 0; i < 4; i++)
	{
		this->preview[i] = false;
		this->z[i] = 0;
	}
	this->baseid = 0;
	this->transform = 0;
	texture = sf::Texture(imageManager.GetImage(baseid));
}

TerrainTile::~TerrainTile()
{

}

void TerrainTile::Draw(sf::Vector2i camOffset, sf::RenderWindow* rw)
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
	for(int i = 0; i < 4; i++)
		if(preview[i])
			quad[i].color = sf::Color(255, 255, 255, 128);
	rw->draw(quad, &texture);
}

int(&TerrainTile::GetHeight())[4]
{
	return z;
}

void TerrainTile::Raise(int elevation, int corner)
{
	if(elevation != 0)
		{
			preview[corner] = false;
			z[corner] += elevation;
		}
	else
		preview[corner] = true;
}

void TerrainTile::SetHeight(int elevation, int corner)
{
	z[corner] = elevation;
}

void TerrainTile::Reset()
{
	for(int i = 0; i < 4; i++)
		preview[i] = false;
}
