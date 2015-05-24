#include "ImageManager.h"

#include <vector>
#include <string>
#include <fstream>

#include <SFML\Graphics.hpp>

#include "rapidxml.hpp"

using namespace rapidxml;

ImageManager::ImageManager()
{

}

ImageManager::~ImageManager()
{

}

void ImageManager::AddImage(sf::Texture& image, int id)
{
	imageList.push_back(image);


	//Map for pairing image ids and the image's index in imageList
	imageIDs[id] = imageList.size() - 1;
}

sf::Texture* ImageManager::GetImage(int id)
{
	return &(imageList[imageIDs[id]]);
}

void ImageManager::LoadTileset(std::string filename)
{
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

	//Some variables used in the following code
	std::string imagePath;
	sf::Image tileset;

	//Go through each imagefile
	xml_node<>* imagefile = root->first_node("imagefile");
	while(imagefile)
	{
		//Get the image file we're parsing and load it
		imagePath = imagefile->first_attribute("path")->value();
		tileset.loadFromFile(imagePath);

		//Go through each tile
		xml_node<>* tile = imagefile->first_node("tile");
		while(tile)
		{
			//Get all the attributes
			int x = atoi(tile->first_attribute("x")->value());
			int y = atoi(tile->first_attribute("y")->value());
			int frames = atoi(tile->first_attribute("frames")->value());
			int size = atoi(tile->first_attribute("size")->value());
			int id = atoi(tile->first_attribute("id")->value());

			//Copy the right tile image from tileset
			sf::Image image;
			image.create(tilesize * size, tilesize * size);
			image.copy(tileset, 0, 0, sf::IntRect(x * tilesize, y * tilesize, frames * size * tilesize, size * tilesize), false);
            sf::Texture tileImage;
            tileImage.loadFromImage(image);

			//Add the image to our image list
			AddImage(tileImage, id);

			//Go to the next tile
			tile = tile->next_sibling();
		}

		//Go to the next imagefile
		imagefile = imagefile->next_sibling();
	}
}
