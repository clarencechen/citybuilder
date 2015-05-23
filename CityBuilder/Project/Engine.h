#ifndef _ENGINE_H
#define _ENGINE_H

#include <SFML\Graphics.hpp>

#include "TerrainTile.h"
#include "ImageManager.h"
#include "Infrastructure.h"
#include "Level.h"

class Engine
{
private:
	static const int tilesize = 32;
	//SFML Render Window
	sf::RenderWindow* window;
	ImageManager imageManager;

	//Render Window (and Camera) size
	sf::Vector2i videoSize;

	//Camera
	sf::View* view;

	//Current Level
	Level* currentLevel;


	sf::Vector2f delta;
	sf::Vector2f target;

	//Initializes the engine
	bool Init();
	//Main Game Loop
	void MainLoop();
	//Renders one frame
	void RenderFrame();
	//Processes user input
	void ProcessInput();
	sf::Vector2i FindTile(sf::Vector2f mouse);
	sf::Vector2i FindPoint(sf::Vector2f mouse);
	void FindCoord(sf::Vector2f& mouse);
	void ProcessKeyInput(sf::Keyboard::Key);
	//Updates all Engine internals
	void Update(float speed);
	//States for building
	sf::IntRect selection;
	sf::Vector2i start;
	int currentZ;
	bool stick;
	bool mouseDown;
	unsigned int mode;
public:
	Engine(int w, int h);
	~Engine();

	void Go();					//Starts the engine
};

#endif
