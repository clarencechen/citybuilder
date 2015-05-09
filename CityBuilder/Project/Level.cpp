#include <vector>
#include <string>
#include <fstream>

#include "Level.h"
#include "Infrastructure.h"
#include "ImageManager.h"
#include "Tile.h"


#include "rapidxml.hpp"

using namespace rapidxml;

Level::Level()
{
}

Level::~Level()
{

}

int Level::GetHeight()
{
	return h;
}

int Level::GetWidth()
{
	return w;
}

void Level::SetDimensions(int w, int h)
{
	map = std::vector<std::vector<TerrainTile*> >(w, std::vector<TerrainTile*>(h, 0));
	buildings = std::vector<std::vector<Building*> >(w, std::vector<Building*>(h, 0));
	bridges = std::vector<std::vector<std::vector<Building*> > >(w, std::vector<std::vector<Building*> >(h, std::vector<Building*>(8, 0)));
}

void Level::AddTile(int x, int y, TerrainTile* tile)
{
	map[x][y] = tile;
}

TerrainTile* Level::GetTile(unsigned int x, unsigned int y)
{
	if(x < map.capacity())
	{
		if(y < map[x].capacity())
		{
			return map[x][y];
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void Level::LoadLevel(std::string filename, ImageManager& imageManager)
{
	//Loads a level from xml file
	//Load the file
	const char* f = filename.c_str();

	std::ifstream inFile(f);

	if(!inFile)
		throw "Could not load tileset: " + filename;

	//Dump contents of file into a string
	std::string xmlContents;

	//Blocked out of preference
	{
		std::string line;
		while(std::getline(inFile, line))
			xmlContents += line;
	}

	//Convert string to rapidxml readable char*
	std::vector<char> xmlData = std::vector<char>(xmlContents.begin(), xmlContents.end());
    xmlData.push_back('\0');

	//Create a parsed document with &xmlData[0] which is the char*
	xml_document<> doc;
	doc.parse<parse_no_data_nodes>(&xmlData[0]);

	//Get the root node
	xml_node<>* root = doc.first_node();

	//Get level attributes
	int width = atoi(root->first_attribute("width")->value());
	int height = atoi(root->first_attribute("height")->value());

	//Resize level
	this->w = width;
	this->h = height;
	SetDimensions(width, height);

	//Load each necessary tileset
	xml_node<>* tileset = root->first_node("tileset");
	while(tileset)
	{
		std::string path = tileset->first_attribute("path")->value();
		//Load tileset
		imageManager.LoadTileset(path);
		//Go to next tileset
		tileset = tileset->next_sibling("tileset");
	}

	//Go through each tile
	xml_node<>* tile = root->first_node("tile");
	//Fill in all tiles with default values
	for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
                TerrainTile* newTile = new TerrainTile(0, i, j, 0, imageManager);
                AddTile(i, j, newTile);
        }
    }
}
