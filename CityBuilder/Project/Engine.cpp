#include "Engine.h"
#include "Level.h"
#include "City.h"
#include "Tile.h"
#include <math.h>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <set>

Engine::Engine(int w, int h)
{
    delta = sf::Vector2f(0,0);
	videoSize = sf::Vector2i(w, h);
	mode = 0;
	selection = sf::IntRect(mode, 0, 0, 0);
	start = sf::Vector2i(0, 0);
    mouseDown = false;
	std::cout << "Finished constructing engine." << std::endl;
}

Engine::~Engine()
{
	delete currentCity;
	delete currentLevel;
	delete view;
	delete window;
}

bool Engine::Init()
{
    counter = 0;
	simSpeed = 2;
	currentZ = 0;
	stick = true;
	sf::Vector2f center(0, 0);
	window = new sf::RenderWindow(sf::VideoMode(videoSize.x, videoSize.y, 32), "Citybuilder");
	sf::Vector2f fVideoSize(videoSize);
	view = new sf::View(center, fVideoSize);
	window->setView(*view);
	if(!window)
		return false;
    std::cout << "Finished constructing window." << std::endl;
	currentLevel = new Level();
	currentLevel->LoadLevel("level1.xml", imageManager);
	currentCity = new City();
	return true;
}

void Engine::RenderFrame()
{
    window->clear();
    currentLevel->Reset();
    if(mode != 0)
        for(int y = 0, tileY = po.selection.top; y <= po.selection.height; y++, tileY++)
            for(int x = 0, tileX = po.selection.left; x <= po.selection.width; x++, tileX++)
                currentLevel->Place(tileX, tileY, currentZ, po.stick, po.mode, true);
	TerrainTile* tile;
	Building* building;//Makes sure we do not draw multiple identical buildings
	std::vector<Draggable*> column;
	//Get the tile bounds we need to draw
	sf::Transform inv(0.5f,-1.f,0.f,0.5f,1.f,0.f,0.f,0.f,0.f);
	sf::Vector2f size(view->getSize()/(float)tilesize);
	sf::Vector2f center(view->getCenter()/(float)tilesize);
	//Transform the center into world coords
	center = inv.transformPoint(center);
	//Set the bounds as the bounding diamond of tiles
	sf::FloatRect bounds(center -.75f*size, 1.5f*size);
	//Figure out how much to offset each tile
	sf::Vector2i camOffset = sf::Vector2i(view->getCenter());
	//Loop through and draw each tile
	//We're keeping track of two variables in each loop. How many tiles
	//we've drawn (x and y), and which tile on the map we're drawing (tileX
	//and tileY)
    for(int y = 0, tileY = (int)(bounds.top); y <= (int)(bounds.height) + 1; y++, tileY++)
	{
		for(int x = 0, tileX = (int)(bounds.left + bounds.width); x <= (int)(bounds.width) + 1; x++, tileX--)
		{
			//Get the tile we're drawing
			tile = currentLevel->GetTile(tileX, tileY);
			if(tile)
				tile->Draw(camOffset, window);
		}
	}
	for(int y = 0, tileY = (int)(bounds.top); y <= (int)(bounds.height) + 1; y++, tileY++)
	{
		for(int x = 0, tileX = (int)(bounds.left + bounds.width); x <= (int)(bounds.width) + 1; x++, tileX--)
		{
			//Get the building we're drawing
			building = currentLevel->GetBuilding(tileX, tileY);
			column = currentLevel->GetBridge(tileX, tileY);
			//check if building exists has not already been drawn
			if(building && !(alreadyDone.count(building->GetAnchor().x*currentLevel->GetHeight() +building->GetAnchor().y)))
            {
                building->Draw(camOffset, window, imageManager);
                alreadyDone.insert(building->GetAnchor().x*currentLevel->GetHeight() +building->GetAnchor().y);
            }
			for(auto& network : column)
				if(network)
					network->Draw(camOffset, window, imageManager);
		}
    }
    alreadyDone.clear();
	window->display();
}

void Engine::ProcessInput()
{
	sf::Event evt;
	//Loop through all window events
	while(window->pollEvent(evt))
	{

		if(evt.type == sf::Event::Closed)
			window->close();
        if(evt.type == sf::Event::KeyPressed)
            ProcessKeyInput(evt.key.code);
		if((evt.type == sf::Event::MouseButtonPressed) && (mouseDown == false))
		{
			mouseDown = true;
			sf::Vector2i coords(FindTile(sf::Vector2f(evt.mouseButton.x, evt.mouseButton.y)));
			sf::Vector2i point(FindPoint(sf::Vector2f(evt.mouseButton.x, evt.mouseButton.y)));
			if(currentLevel->GetTile(coords.x, coords.y))
			{
				if(mode == 0)
				{
					if(currentLevel->GetBuilding(coords.x, coords.y))
                    {
						std::cout   << "This Tile: " << currentLevel->GetBuilding(coords.x, coords.y)->GetRoad()
                                    << " " << currentLevel->GetBuilding(coords.x, coords.y)->GetRail()
                                    << " " << currentLevel->GetBuilding(coords.x, coords.y)->GetId()
                                    << " " << currentLevel->GetBuilding(coords.x, coords.y)->GetDisplayTile(currentLevel).x
                                    << " " << currentLevel->GetBuilding(coords.x, coords.y)->GetDisplayTile(currentLevel).y << std::endl;
                        std::cout   << "Residents/Employees/People Served: " << currentLevel->GetBuilding(coords.x, coords.y)->GetPop().x << std::endl;
                        std::cout   << "Stored Goods: " << currentLevel->GetBuilding(coords.x, coords.y)->GetPop().y << std::endl;
                        std::cout   << "Production: " << currentLevel->GetBuilding(coords.x, coords.y)->GetPop().z << std::endl;
                    }

					else
						std::cout << "This Tile is vacant." << std::endl;
				}
				else if(mode < 16 || mode == 64)
				{
					start = sf::Vector2i(coords.x, coords.y);
					selection = sf::IntRect(coords.x, coords.y, 0, 0);
				}
				//terraforming
				else if(mode == 65 || mode == 66)
				{
					start = sf::Vector2i(point.x, point.y);
					selection = sf::IntRect(point.x, point.y, 0, 0);
				}
			}
		}
		if(evt.type == sf::Event::MouseMoved)
		{
			sf::Vector2i coords(FindTile(sf::Vector2f(evt.mouseMove.x, evt.mouseMove.y)));
			sf::Vector2i point(FindPoint(sf::Vector2f(evt.mouseMove.x, evt.mouseMove.y)));
			if(mouseDown)
			{
				//bulldoze
				if(mode >= 64)
				{
					//make rectangle have positive width and height
					int dx = mode == 64 ? coords.x -start.x : point.x -start.x;
					int dy = mode == 64 ? coords.y -start.y : point.y -start.y;
					if(dy < 0)
					{
						if(dx < 0)
							selection = mode == 64 ? sf::IntRect(coords.x, coords.y, -dx, -dy) : sf::IntRect(point.x, point.y, -dx, -dy);
						else
							selection = mode == 64 ? sf::IntRect(start.x, coords.y, dx, -dy) : sf::IntRect(start.x, point.y, dx, -dy);
					}
					else
					{
						if(dx < 0)
							selection = mode == 64 ? sf::IntRect(coords.x, start.y, -dx, dy) : sf::IntRect(point.x, start.y, -dx, dy);
						else
							selection = mode == 64 ? sf::IntRect(start.x, start.y, dx, dy) : sf::IntRect(start.x, start.y, dx, dy);
					}
                    po = PreviewOrder(selection, mode, stick);
				}
				//transport
				else if (mode > 0 && mode < 16)
				{
					int dx = coords.x -start.x;
					int dy = coords.y -start.y;
					if(dy > dx)
					{
						if(dy > -dx)
							selection = sf::IntRect(start.x, start.y, 0, dy);
						else
							selection = sf::IntRect(coords.x, start.y, -dx, 0);
					}
					else
					{
						if(dy > -dx)
							selection = sf::IntRect(start.x, start.y, dx, 0);
						else
							selection = sf::IntRect(start.x, coords.y, 0, -dy);
					}
                    po = PreviewOrder(selection, mode, stick);
				}
				else if(mode > 0 && mode < 64)
					po = PreviewOrder(sf::IntRect(coords.x, coords.y, 0, 0), mode, stick);
			}
			else if(mode > 0)
				po = (mode == 65 || mode == 66) ? PreviewOrder(sf::IntRect(point.x, point.y, 0, 0), mode, stick)
                                                : PreviewOrder(sf::IntRect(coords.x, coords.y, 0, 0), mode, stick);
		}
		if(evt.type == sf::Event::MouseButtonReleased)
		{
			sf::Vector2i coords(FindTile(sf::Vector2f(evt.mouseButton.x, evt.mouseButton.y)));
			if((mode > 0 && mode < 16) || mode >= 64)
				for(int y = 0, tileY = selection.top; y <= selection.height; y++, tileY++)
					for(int x = 0, tileX = selection.left; x <= selection.width; x++, tileX++)
						currentLevel->Place(tileX, tileY, currentZ, stick, mode, false);
			else if(mode > 0 && mode < 64)
			{
				sf::Vector2i coords(FindTile(sf::Vector2f(evt.mouseButton.x, evt.mouseButton.y)));
				currentLevel->Place(coords.x, coords.y, currentZ, stick, mode, false);
			}
			mouseDown = false;
		}
	}
}

sf::Vector2i Engine::FindTile(sf::Vector2f mouse)
{
	FindCoord(mouse);
	//edit later, not completely accurate
	mouse += sf::Vector2f(-0.5, 0.5);
	return sf::Vector2i(mouse);
}

sf::Vector2i Engine::FindPoint(sf::Vector2f mouse)
{
	FindCoord(mouse);
	return sf::Vector2i(mouse);
}

void Engine::FindCoord(sf::Vector2f& mouse)
{
	mouse -= view->getSize() / 2.0f;
	sf::Vector2f center(view->getCenter().x, view->getCenter().y);
	mouse += center;
	mouse /= (float)tilesize;
	sf::Transform inv(0.5f,-1.f,0.f,0.5f,1.f,0.f,0.f,0.f,0.f);
	mouse = inv.transformPoint(mouse.x, mouse.y);
	mouse += sf::Vector2f(0.5, 0.5);
	int height = currentLevel->GetHeightForMouse((int)mouse.x, (int)mouse.y);
	mouse += sf::Vector2f(-(float)height/4, (float)height/4);
}

void Engine::ProcessKeyInput(sf::Keyboard::Key code)
{
        if(code == sf::Keyboard::Left)
            delta = sf::Vector2f(-view->getSize().x, 0) / 25.0f;
        else if(code == sf::Keyboard::Right)
            delta = sf::Vector2f(view->getSize().x, 0) / 25.0f;
        else if(code == sf::Keyboard::Up)
            delta = sf::Vector2f(0, -view->getSize().y) / 25.0f;
        else if(code == sf::Keyboard::Down)
            delta = sf::Vector2f(0, view->getSize().y) / 25.0f;
        //small road
        else if(code == sf::Keyboard::R)
            mode = 1;
        //large road
        else if(code == sf::Keyboard::Num4)
			mode = 2;
		//rail
		else if(code == sf::Keyboard::T)
			mode = 3;
		//museum
		else if(code == sf::Keyboard::M)
			mode = 32;
		//elementary school
		else if(code == sf::Keyboard::K)
			mode = 33;
		//police station
		else if(code == sf::Keyboard::P)
			mode = 34;
		//fire station
		else if(code == sf::Keyboard::F)
			mode = 35;
		//hospital
		else if(code == sf::Keyboard::H)
			mode = 36;
		//high school
		else if(code == sf::Keyboard::J)
			mode = 37;
        //demolish
        else if(code == sf::Keyboard::Escape)
            mode = 0;
        //query
        else if(code == sf::Keyboard::Tilde)
			mode = 64;
		//lower terrain
		else if(code == sf::Keyboard::Comma)
			mode = 65;
		//raise terrain
		else if(code == sf::Keyboard::Period)
			mode = 66;
        //City Stats
        else if(code == sf::Keyboard::Slash)
            currentCity->DumpStats();
        //medium residential zone
        else if(code == sf::Keyboard::A)
            mode = 19;
        //medium commercial zones
        else if(code == sf::Keyboard::S)
            mode = 20;
        //medium industrial zones
        else if(code == sf::Keyboard::D)
            mode = 21;
        else if(code == sf::Keyboard::Num1)
        {
            simSpeed = 1;
            std::cout << "Speed set to slow." << std::endl;
        }
        else if(code == sf::Keyboard::Num2)
        {
            simSpeed = 2;
            std::cout << "Speed set to medium." << std::endl;
        }
        else if(code == sf::Keyboard::Num3)
        {
            simSpeed = 4;
            std::cout << "Speed set to fast." << std::endl;
        }
        else if(code == sf::Keyboard::Space)
        {
            simSpeed = 0;
            std::cout << "Simulation paused." << std::endl;
        }
		else if(code == sf::Keyboard::PageUp)
		{
			currentZ++;
			std::cout << "Current Z: " << currentZ << std::endl;
		}
		else if(code == sf::Keyboard::PageDown)
		{
			currentZ--;
			std::cout << "Current Z: " << currentZ << std::endl;
		}
		else if(code == sf::Keyboard::BackSlash)
		{
		    stick = !stick;
		    std::cout << "Bridge state toggled." << std::endl;
		}
}

void Engine::MoveCamera(float speed)
{
    //X distance to target, Y distance to target, and Euclidean distance
	float d = sqrt(delta.x*delta.x + delta.y*delta.y);
	//Velocity magnitudes
    float v = 0.0f;
	//If we're within 1 pixel of the target already, just snap
	//to target and stay there. Otherwise, continue
	if((d*d) <= 1)
		delta = sf::Vector2f(0,0);
	else
	{
		//We set our velocity to move 1/60th of the distance to
		//the target. 60 is arbitrary, I picked it because I intend
		//to run this function once every 60th of a second. We also
		//allow the user to change the camera speed via the speed member
		v = (d * speed)/60.0f;

		//Keep v above 1 pixel per update, otherwise it may never get to
		//the target. v is an absolute value thanks to the squaring of x
		//and y earlier
		if(v < 1.0f)
			v = 1.0f;

		//Similar triangles to get vx and vy
		sf::Vector2f vc(delta * (v/d));
        view->move(vc);
        delta = delta - vc;
    }
}

void Engine::Simulate(int simSpeed)
{
    if(simSpeed)
        counter = (counter/simSpeed)*simSpeed;
    counter += simSpeed;
    for(int i = 0; i <= simSpeed; ++i)
        currentCity->Update(currentLevel);
    if(counter % 256 == 0)
    {
        currentLevel->Shuffle();
        currentCity->Budget();
        std::cout << "Cycle " << counter/256 << " has arrived." << std::endl;
    }
}

void Engine::MainLoop()
{
	//Loop until our window is closed
	while(window->isOpen())
	{
		ProcessInput();
        MoveCamera(15.0f);
        Simulate(simSpeed);
		RenderFrame();
    }
}

void Engine::Go()
{
	if(!Init())
		throw "Could not initialize Engine";

	MainLoop();
}

