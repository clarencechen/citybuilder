#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
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
	map = std::vector<std::vector<TerrainTile*> >(w, std::vector<TerrainTile*>(h));
	buildings = std::vector<std::vector<Building*> >(w, std::vector<Building*>(h));
	bridges = std::vector<std::vector<std::vector<Draggable*> > >(w, std::vector<std::vector<Draggable*> >(h, std::vector<Draggable*>(8)));
	Shuffle();
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
    std::cout << "Finished dumping file." << std::endl;
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
    std::cout << "Finished loading tileset." << std::endl;
	//Go through each tile
	xml_node<>* tile = root->first_node("tile");
	//Fill in all tiles with default values
	tex = imageManager.GetImage(0);
	for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
            map[i][j] = new TerrainTile(i, j, tex);
     //   std::cout << "Created tile row " << i << std::endl;
    }
}
void Level::Shuffle()
{
    while(this->shuffledInts.size() < (w*h))
        this->shuffledInts.push_back(0);
    std::iota(shuffledInts.begin(), shuffledInts.end(), 1);
    std::random_shuffle(shuffledInts.begin(), shuffledInts.end());
    return;
}
