#ifndef _IMAGEMANAGER_H
#define _IMAGEMANAGER_H

#include <vector>
#include <map>
#include <string>
#include <SFML\Graphics.hpp>

class ImageManager
{
private:
	std::vector<sf::Texture> imageList;
	std::map<int, int> imageIDs;
public:
	static const int tilesize = 32;
	ImageManager();
	~ImageManager();
	void AddImage(sf::Texture& image, int id);
	sf::Texture& GetImage(int id);

	//Loads tileset from xml format
	void LoadTileset(std::string filename);
};

#endif
