#include "Engine.h"
#include "Level.h"
#include "Infrastructure.h"
#include "Tile.h"
#include "Building.h"
#include <math.h>
#include <SFML\Graphics.hpp>
#include <iostream>

Engine::Engine(int w, int h)
{
	delta = sf::Vector2f(0,0);
	videoSize = sf::Vector2i(w, h);
	mode = 128;
	selection = sf::IntRect(0, 0, 0, 0);
	start = sf::Vector2i(0, 0);
}

Engine::~Engine()
{

}

bool Engine::Init()
{
	currentZ = 0;
	stick = true;
	sf::Vector2f center(0, 0);
	window = new sf::RenderWindow(sf::VideoMode(videoSize.x, videoSize.y, 32), "RPG");
	sf::Vector2f fVideoSize(videoSize);
	view = new sf::View(center, fVideoSize);
	window->setView(*view);
	mouseDown = false;

	if(!window)
		return false;

	currentLevel = new Level();
	currentLevel->LoadLevel("level1.xml", imageManager);
	return true;
}

void Engine::RenderFrame()
{
	TerrainTile* tile;
	Building* building;
	Building* last = 0; //Makes sure we do not draw multiple identical buildings
	std::vector<Building*> column;
	window->clear();
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
//			column = currentLevel->GetBridge(tileX, tileY);
			if(tile)
				tile->Draw(camOffset, window);
//			for(auto& road : column)
//				if(road)
//					road->Draw(camOffset, window);
		}
	}
	for(int y = 0, tileY = (int)(bounds.top); y <= (int)(bounds.height) + 1; y++, tileY++)
	{
		for(int x = 0, tileX = (int)(bounds.left + bounds.width); x <= (int)(bounds.width) + 1; x++, tileX--)
		{
			//Get the building we're drawing
			building = currentLevel->GetBuilding(tileX, tileY);
			column = currentLevel->GetBridge(tileX, tileY);
			if(building && building != last)
				building->Draw(camOffset, window);
			for(auto& road : column)
				if(road)
					road->Draw(camOffset, window);
			last = building;
		}
	}
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
				if(mode == 128)
				{
					if(currentLevel->GetBuilding(coords.x, coords.y))
						std::cout << "This Tile: " << currentLevel->GetId(coords.x, coords.y, 0, true) << "  " << currentLevel->GetBuildingTile(coords.x, coords.y, currentZ, true).x << std::endl;
					else
						std::cout << "This Tile is vacant." << std::endl;
				}
				else if(mode <= 16)
				{
					start = sf::Vector2i(coords.x, coords.y);
					selection = sf::IntRect(coords.x, coords.y, 0, 0);
				}
				//terraforming
				else if(mode == 129 || mode == 130)
				{
					start = sf::Vector2i(point.x, point.y);
					selection = sf::IntRect(point.x, point.y, 0, 0);
				}
			}
		}
		else if(evt.type == sf::Event::MouseMoved)
		{
			currentLevel->Reset(imageManager);

			sf::Vector2i coords(FindTile(sf::Vector2f(evt.mouseMove.x, evt.mouseMove.y)));
			sf::Vector2i point(FindPoint(sf::Vector2f(evt.mouseMove.x, evt.mouseMove.y)));
			if(mouseDown)
			{
				//bulldoze
				if(mode == 0)
				{
					//make rectangle have positive width and height
					int dx = coords.x -start.x;
					int dy = coords.y -start.y;
					if(dy < 0)
					{
						if(dx < 0)
							selection = sf::IntRect(coords.x, coords.y, -dx, -dy);
						else
							selection = sf::IntRect(start.x, coords.y, dx, -dy);

					}
					else
					{
						if(dx < 0)
							selection = sf::IntRect(coords.x, start.y, -dx, dy);
						else
							selection = sf::IntRect(start.x, start.y, dx,  dy);
					}
					for(int y = 0, tileY = selection.top; y <= selection.height; y++, tileY++)
					{
						for(int x = 0, tileX = selection.left; x <= selection.width; x++, tileX++)
						{
							currentLevel->Place(tileX, tileY, currentZ, stick, mode, true, imageManager);
						}
					}
				}
				//transport
				else if (mode > 0 && mode <= 16)
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
					for(int y = 0, tileY = selection.top; y <= selection.height; y++, tileY++)
					{
						for(int x = 0, tileX = selection.left; x <= selection.width; x++, tileX++)
							currentLevel->Place(tileX, tileY, currentZ, stick, mode, true, imageManager);
					}
				}
				else if(mode < 128)
				{
					currentLevel->Place(coords.x, coords.y, currentZ, stick, mode, true, imageManager);
				}
 				//terraforming (129 and 130 kept separate in case different coloring is needed)
				else if(mode == 129)
				{
					int dx = point.x -start.x;
					int dy = point.y -start.y;
					if(dy < 0)
					{
						if(dx < 0)
							selection = sf::IntRect(point.x, point.y, -dx, -dy);
						else
							selection = sf::IntRect(start.x, point.y, dx, -dy);

					}
					else
					{
						if(dx < 0)
							selection = sf::IntRect(point.x, start.y, -dx, dy);
						else
							selection = sf::IntRect(start.x, start.y, dx,  dy);
					}
					for(int y = 0, pointY = selection.top; y <= selection.height; y++, pointY++)
					{
						for(int x = 0, pointX = selection.left; x <= selection.width; x++, pointX++)
							currentLevel->Terraform(pointX, pointY, 0);
					}
				}
				else if (mode == 130)
				{
					int dx = point.x -start.x;
					int dy = point.y -start.y;
					if(dy < 0)
					{
						if(dx < 0)
							selection = sf::IntRect(point.x, point.y, -dx, -dy);
						else
							selection = sf::IntRect(start.x, point.y, dx, -dy);
					}
					else
					{
						if(dx < 0)
							selection = sf::IntRect(point.x, start.y, -dx, dy);
						else
							selection = sf::IntRect(start.x, start.y, dx,  dy);
					}
					for(int y = 0, pointY = selection.top; y <= selection.height; y++, pointY++)
					{
						for(int x = 0, pointX = selection.left; x <= selection.width; x++, pointX++)
							currentLevel->Terraform(pointX, pointY, 0);
					}
				}
			}
			else if(mode > 0 && mode <= 16)
			{
				currentLevel->Place(coords.x, coords.y, currentZ, stick, mode, true, imageManager);
			}
			else if(mode == 129 || mode == 130)
			{
				sf::Vector2i point(FindPoint(sf::Vector2f(evt.mouseMove.x, evt.mouseMove.y)));
				currentLevel->Terraform(point.x, point.y, 0);
			}
			else
			{
				currentLevel->Place(coords.x, coords.y, currentZ, stick, mode, true, imageManager);
			}
		}
		else if(evt.type == sf::Event::MouseButtonReleased)
		{
			sf::Vector2i point(FindPoint(sf::Vector2f(evt.mouseMove.x, evt.mouseMove.y)));
			sf::Vector2i coords(FindTile(sf::Vector2f(evt.mouseButton.x, evt.mouseButton.y)));
			if(mode <= 16)
			{
				for(int y = 0, tileY = selection.top; y <= selection.height; y++, tileY++)
				{
					for(int x = 0, tileX = selection.left; x <= selection.width; x++, tileX++)
						currentLevel->Place(tileX, tileY, currentZ, stick, mode, false, imageManager);
				}
			}
			else if(mode < 128)
			{
				sf::Vector2i coords(FindTile(sf::Vector2f(evt.mouseButton.x, evt.mouseButton.y)));
				currentLevel->Place(coords.x, coords.y, currentZ, stick, mode, false, imageManager);
			}
			else if(mode == 129)
			{
				for(int y = 0, pointY = selection.top; y <= selection.height; y++, pointY++)
				{
					for(int x = 0, pointX = selection.left; x <= selection.width; x++, pointX++)
						currentLevel->Terraform(pointX, pointY, 1);
				}
			}
			else if (mode == 130)
			{
				for(int y = 0, pointY = selection.top; y <= selection.height; y++, pointY++)
				{
					for(int x = 0, pointX = selection.left; x <= selection.width; x++, pointX++)
						currentLevel->Terraform(pointX, pointY, -1);
				}
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
        {
            delta = sf::Vector2f(-view->getSize().x, 0) / 25.0f;
        }
        else if(code == sf::Keyboard::Right)
        {
            delta = sf::Vector2f(view->getSize().x, 0) / 25.0f;
        }
        else if(code == sf::Keyboard::Up)
        {
            delta = sf::Vector2f(0, -view->getSize().y) / 25.0f;
        }
        else if(code == sf::Keyboard::Down)
        {
            delta = sf::Vector2f(0, view->getSize().y) / 25.0f;
        }
        //small road
        else if(code == sf::Keyboard::R)
        {
            mode = 1;
        }
        //large road
        else if(code == sf::Keyboard::Num4)
		{
			mode = 2;
		}
		//museum
		else if(code == sf::Keyboard::M)
		{
			mode = 32;
		}
		//elementary school
		else if(code == sf::Keyboard::K)
		{
			mode = 33;
		}
		//police station
		else if(code == sf::Keyboard::P)
		{
			mode = 34;
		}
		//fire station
		else if(code == sf::Keyboard::F)
		{
			mode = 35;
		}
		//hospital
		else if(code == sf::Keyboard::H)
		{
			mode = 36;
		}
		//library
		else if(code == sf::Keyboard::L)
		{
			mode = 37;
		}
        //demolish
        else if(code == sf::Keyboard::Escape)
        {
            mode = 128;
        }
        //query
        else if(code == sf::Keyboard::Tilde)
		{
			mode = 0;
		}
		//lower terrain
		else if(code == sf::Keyboard::Comma)
		{
			mode = 129;
		}
		//raise terrain
		else if(code == sf::Keyboard::Period)
		{
			mode = 130;
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
		}
}

void Engine::Update(float speed)
{
    //X distance to target, Y distance to target, and Euclidean distance
	float d = sqrt(delta.x*delta.x + delta.y*delta.y);
	//Velocity magnitudes
    float v = 0.0f;
	//If we're within 1 pixel of the target already, just snap
	//to target and stay there. Otherwise, continue
	if((d*d) <= 1)
	{
		delta = sf::Vector2f(0,0);
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

		//Similar triangles to get vx and vy
		sf::Vector2f vc(delta * (v/d));
        view->move(vc);
        delta = delta - vc;
    }
}

void Engine::MainLoop()
{
	//Loop until our window is closed
	while(window->isOpen())
	{
		ProcessInput();
		Update(15);
		RenderFrame();
	}
}

void Engine::Go()
{
	if(!Init())
		throw "Could not initialize Engine";

	MainLoop();
}
