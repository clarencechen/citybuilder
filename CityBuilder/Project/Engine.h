#ifndef _ENGINE_H
#define _ENGINE_H

#include <SFML\Graphics.hpp>
#include <TGUI\TGUI.hpp>
#include <set>
#include "Level.h"
#include "City.h"
#include "ImageManager.h"
#include "Tile.h"

struct PreviewOrder
{
    sf::IntRect selection;
    unsigned int mode;
    bool stick;
    PreviewOrder()
    {
        selection = sf::IntRect(0,0,0,0);
        mode = 0;
        stick = true;
    }
    PreviewOrder(sf::IntRect s, unsigned int r, bool stick)
    {
        selection = s;
        mode = r;
        this->stick = stick;
    }
};

class Engine
{
private:
	std::set<unsigned int> alreadyDone;
	static const int tilesize = 32;
	const std::string menutext[9][7] = {{"Terraforming", "Transport", "Zoning", "Utilities", "Services", "City", "File"},
										{"Raise Terrain", "Small Road", "Residential Zone", "Power Plant", "Fire Station", "Query", "New City"},
										{"Lower Terrain", "Large Road", "Commercial Zone", "Power Lines", "Police Station", "Demolish", "Load City"},
										{"Plant Trees", "Small One-Way Road", "Industrial Zone", "Water Pump", "Elementary School", "Budget", "Save City"},
										{"Place Water", "Large One-Way Road", "", "Water Pipes", "High School", "Statistics", "Game Options"},
										{"Set Reserve", "Light Rail/Metro", "", "Sewer Outflow", "Museum", "Data Overlay", ""},
										{"", "LRT/Metro Station", "", "Sewer Treatment", "Library", "Trip Query", ""},
										{"", "Bus Stop", "", "Landfill", "Hospital", "", ""},
										{"", "", "", "Recycling Center", "Park", "", ""}
									   };
	//SFML Render Window
	sf::RenderWindow* window;
	ImageManager imageManager;
	tgui::Gui* gui;
	tgui::MenuBar::Ptr menu;

	//Render Window (and Camera) size
	sf::Vector2i videoSize;

    //counter used for simulation;
    long long int counter;

	//Camera
	sf::View* view;

	//Current Level/Map
	Level* currentLevel;
	City* currentCity;
    //stores current preview order each cycle;
    PreviewOrder po;
	sf::Vector2f delta;
	float epsilon;
	//Initializes the engine
	bool Init();
	//Main Game Loop
	void MainLoop();
	//Renders one frame
	void RenderFrame();
	//Processes user input
	void ProcessInput();
	//Simulate changes
	void Simulate(int speed);
	sf::Vector2i FindTile(sf::Vector2f mouse);
	sf::Vector2i FindPoint(sf::Vector2f mouse);
	void FindCoord(sf::Vector2f& mouse);
	void ProcessMenuInput(const tgui::Callback& callback);
	void ProcessKeyInput(sf::Keyboard::Key);
	//Updates all Engine internals
	void MoveCamera(float speed);
	void ZoomCamera(float speed);
	//simulation speed
	int simSpeed;
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
