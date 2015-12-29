#include "Engine.h"
#include "Level.h"
#include "City.h"
#include "Tile.h"
#include <math.h>
#include <SFML\Graphics.hpp>
#include <TGUI\TGUI.hpp>
#include <iostream>
#include <set>

Engine::Engine(int w, int h)
{
    delta = sf::Vector2f(0,0);
    epsilon = 1;
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
	delete gui;
	delete view;
	delete window;
}

bool Engine::Init()
{
    counter = 0;
	simSpeed = 2;
	currentZ = 0;
	stick = true;
	window = new sf::RenderWindow(sf::VideoMode(videoSize.x, videoSize.y, 32), "Citybuilder");

	sf::Vector2f fVideoSize(videoSize);
	view = new sf::View(sf::Vector2f(0, 0), fVideoSize);
	window->setView(*view);

	gui = new tgui::Gui(*window);
	gui->setGlobalFont("..\\..\\..\\..\\TGUI-0.7\\fonts\\DejaVuSans.ttf");
	//what if directory needs to change?

	if(!window || !gui)
		return false;
    std::cout << "Finished constructing window and GUI." << std::endl;

	currentLevel = new Level();
	currentLevel->LoadLevel("level1.xml", imageManager);
	currentCity = new City();

	menu = std::make_shared<tgui::MenuBar>();
	menu->setSize(view->getSize().x, 20);
	menu->setPosition(0, 0);
	for(int i = 0; i < 7; i++)
	{
		menu->addMenu(menutext[0][i]);
		for(int j = 1; j < 9; j++)
			if(menutext[j][i].compare("") != 0)
				menu->addMenuItem(menutext[0][i], menutext[j][i]);
	}
	menu->connectEx("MenuItemClicked", &Engine::ProcessMenuInput, this);
	gui->add(menu, "Menu");
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
	//rendering starts at (0,64) and goes down diagonally by x = y + a where a decreases from w to -w
	int d = currentLevel->GetWidth() -currentLevel->GetHeight();
	for(int a = currentLevel->GetWidth() - 1; a > -currentLevel->GetHeight(); a--)
	{
		for(int y = a < 0 ? -a : 0; y < (d < 0
										? (a > d ? currentLevel->GetHeight() : currentLevel->GetHeight() + a)
										: (a < d ? currentLevel->GetHeight() : currentLevel->GetWidth() - a)); y++)
		{
			//Get the tile we're drawing
			tile = currentLevel->GetTile(y +a, y);
			if(tile)
				tile->Draw(window);
		}
	}
	for(int a = currentLevel->GetWidth() - 1; a > -currentLevel->GetHeight(); a--)
	{
		for(int y = a < 0 ? -a : 0; y < (d < 0
										? (a > d ? currentLevel->GetHeight() : currentLevel->GetHeight() + a)
										: (a < d ? currentLevel->GetHeight() : currentLevel->GetWidth() - a)); y++)
		{
			//Get the building we're drawing
			building = currentLevel->GetBuilding(y +a, y);
			column = currentLevel->GetBridge(y +a, y);
			//check if building exists has not already been drawn
			if(building && !(alreadyDone.count(building->GetAnchor().x*currentLevel->GetHeight() +building->GetAnchor().y)))
            {
                building->Draw(window, imageManager);
                alreadyDone.insert(building->GetAnchor().x*currentLevel->GetHeight() +building->GetAnchor().y);
            }
			for(auto& network : column)
				if(network)
					network->Draw(window, imageManager);
		}
    }
    alreadyDone.clear();
    gui->draw();
	window->display();
}

void Engine::ProcessInput()
{
	sf::Event evt;
	//Loop through all window events
	while(window->pollEvent(evt))
	{
		gui->handleEvent(evt);
		if(evt.type == sf::Event::Closed)
			window->close();
		if(evt.type == sf::Event::Resized)
		{
			view->setSize(evt.size.width, evt.size.height);
			window->setView(*view);
		}
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
		if(evt.type == sf::Event::MouseWheelScrolled)
		{
			if(evt.mouseWheelScroll.delta < 0)
				epsilon *= sqrt(2.f);
			else
				epsilon *= sqrt(0.5f);
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
	mouse = window->mapPixelToCoords(sf::Vector2i(mouse));
	mouse /= (float)tilesize;
	sf::Transform inv(0.5f,-1.f,0.f,0.5f,1.f,0.f,0.f,0.f,0.f);
	mouse = inv.transformPoint(mouse.x, mouse.y);
	mouse += sf::Vector2f(0.5, 0.5);
	int height = currentLevel->GetHeight((int)mouse.x, (int)mouse.y, 1);
	mouse += sf::Vector2f(-(float)height/4, (float)height/4);
}
void Engine::ProcessMenuInput(const tgui::Callback& callback)
{
	switch(callback.index)
	{
	case 0:
		if(callback.text == "Raise Terrain")
			mode = 65;
		else if(callback.text == "Lower Terrain")
			mode = 66;
		break;
	case 1://modes 1 to 12 and 25 to 31
		if(callback.text == "Small Road")
			mode = 1;
		else if(callback.text == "Large Road")
			mode = 2;
		else if(callback.text == "Light Rail/Metro")
			mode = 8;
		break;
	case 2://modes 16 to 24
		if(callback.text == "Residential Zone")
			mode = 19;
		else if(callback.text == "Commercial Zone")
			mode = 20;
		else if(callback.text == "Industrial Zone")
			mode = 21;
		break;
	case 3://modes 13 to 16 and 48 to 63
		if(callback.text == "Power Plant")
			mode = 44;
		break;
	case 4://modes 32 to 47
		if(callback.text == "Fire Station")
			mode = 35;
		else if(callback.text == "Police Station")
			mode = 34;
		else if(callback.text == "Elementary School")
			mode = 33;
		else if(callback.text == "High School")
			mode = 37;
		else if(callback.text == "Museum")
			mode = 32;
		//else if(callback.text == "Library")
			//mode = 38;
		else if(callback.text == "Hospital")
			mode = 36;
		//else if(callback.text == "Park")
			//mode = 39;
		break;
	case 5:
		if(callback.text == "Query")
			mode = 0;
		else if(callback.text == "Demolish")
			mode = 64;
	}
}
void Engine::ProcessKeyInput(sf::Keyboard::Key code)
{
        if(code == sf::Keyboard::Left)
            delta += sf::Vector2f(-view->getSize().x, 0) / 25.0f;
        else if(code == sf::Keyboard::Right)
            delta += sf::Vector2f(view->getSize().x, 0) / 25.0f;
        else if(code == sf::Keyboard::Up)
            delta += sf::Vector2f(0, -view->getSize().y) / 25.0f;
        else if(code == sf::Keyboard::Down)
            delta += sf::Vector2f(0, view->getSize().y) / 25.0f;
        //small road
        else if(code == sf::Keyboard::R)
            mode = 1;
        //large road
        else if(code == sf::Keyboard::Num4)
			mode = 2;
		//rail
		else if(code == sf::Keyboard::T)
			mode = 8;
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
		//power plant
		else if(code == sf::Keyboard::V)
			mode = 44;
        //query
        else if(code == sf::Keyboard::Escape)
            mode = 0;
        //demolish
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
    //Euclidean distance
	float d = sqrt(delta.x*delta.x + delta.y*delta.y);
	//Velocity magnitudes
    float v = 0.f;
	//If we're within 1 pixel of the target already, just snap
	//to target and stay there. Otherwise, continue
	if((d*d) <= 1)
	{
		view->move(delta);
		window->setView(*view);
		delta = sf::Vector2f(0.f, 0.f);
	}
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

		//Similar triangles to get vc
		sf::Vector2f vc((delta/d)*v);
        view->move(vc);
        window->setView(*view);
        delta -= vc;
    }
}

void Engine::ZoomCamera(float speed)
{
	float v = 0.f;
	if(fabs(1 -epsilon) <= 1e-4)
	{
		view->zoom(epsilon);
		window->setView(*view);
		epsilon = 1;
	}
	else
	{
		v = pow(epsilon, speed/60.);
		view->zoom(v);
		window->setView(*view);
		epsilon /= v;
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
        ZoomCamera(15.0f);
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

