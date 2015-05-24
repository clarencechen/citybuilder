#include "Level.h"
#include "Engine.h"
#include "Building.h"

#include <SFML\Graphics.hpp>
#include <iostream>

void Level::Place(int x, int y, int z, bool stick, unsigned int r, bool preview)
{
    //catch for out of bounds
    if (x < 0 || y < 0 || x >= w || y >= h)
        return;
    else
    {
		Building* b = stick ? GetBuilding(x, y) : GetBridge(x, y, z);
		if(r == 64 && b) //delete tool
		{
			sf::Vector2u anchor = b->GetAnchor();
			if(preview)
            {
 				b->Condemn();
                return;
            }
			else if(!stick)
			{
				delete b;
				b = 0;
			}
            else if(stick)
			{
				sf::Vector2u footprint = b->GetFootprint();
				delete b;
				for(int i = 0; i < footprint.x; i++)
					for(int j = 0; j < footprint.y; j++)
						buildings[anchor.x + i][anchor.y - j] = 0;
			}
			//Update display tiles
			for(int i = anchor.x - 1; i <= anchor.x + 1; i++)
			{
				for(int j = anchor.y - 1; j <= anchor.y + 1; j++)
				{
					Building* n = stick ? GetBuilding(i, j) : GetBridge(i, j, z);
					if(n)
					{
						n->MatchNetwork(preview, this);
						n->SetStatus(n->GetDisplayTile(this), preview);
					}
				}
			}
		}
		else if(r < 16)
        {
			Building* d = stick ? GetBuilding(x, y) : GetBridge(x, y, z);
			if(!d)
			{
				if(!stick)
					bridges[x][y][z] = new Draggable(r, x, y, z, preview, this);
				else
					buildings[x][y] = new Draggable(r, x, y, GetTile(x, y)->GetHeight(), preview, this);
			}
			else
				d->Add(r, preview);
            //Update display tiles
			for(int i = x - 1; i <= x + 1; i++)
			{
				for(int j = y - 1; j <= y + 1; j++)
				{
					Building* n = stick ? GetBuilding(i, j) : GetBridge(i, j, z);
					if(n)
					{
						n->MatchNetwork(preview, this);
						n->SetStatus(n->GetDisplayTile(this), preview);
					}
				}
			}
        }
        else if(r >= 16 && r < 64)
        {
			for(int i = 0; i < footprint[r].x; i++)
			{
				for(int j = 0; j < footprint[r].y; j++)
				{
					if(x + i < 0 || y - j < 0 || x + i >= w || y - j >= h)
					{
						if(!preview)
							std::cout << "Building footprint out of bounds" << std::endl;
						return;
					}
					else if(GetBuilding(x + i, y - j) && !GetBuilding(x + i, y - j)->GetPreview())
					{
						if(!preview)
							std::cout << "Tile occupied" << std::endl;
						return;
					}
				}
			}
            r < 32 ?    buildings[x][y] = new Zone(r, x, y, GradeBuilding(sf::IntRect(x, y -footprint[r].y, footprint[r].x, footprint[r].y)), preview) :
                        buildings[x][y] = new Structure(r, x, y, GradeBuilding(sf::IntRect(x, y -footprint[r].y, footprint[r].x, footprint[r].y)), preview);
            for(int i = 0; i < footprint[r].x; i++)
                for(int j = 0; j < footprint[r].y; j++)
                    buildings[x + i][y - j] = buildings[x][y];
        buildings[x][y]->SetStatus(buildings[x][y]->GetDisplayTile(this), preview);
        }
        else if(r == 65 || r == 66)
        {
            if(preview)
                Terraform(x, y, 0);
            else
                Terraform(x, y, r == 65 ? 1 : -1);
        }
    }
}

void Level::Reset()
{
    for(auto& vector : bridges)
    {
        for(auto& vec : vector)
        {
            for(Draggable*& bridge : vec)
            {
                if(bridge)
                {
                    if(bridge->GetDel())
                    {
                        delete bridge;
                        bridge = 0;
                    }
                    else
                        bridge->Reset();
                }
            }
        }
    }
    for(int x = 0; x < w; ++x)
    {
        for(int y = h - 1; y >= 0; --y)
        {
            Building* b = GetBuilding(x, y);
            if(b && b->GetDel())
			{
                sf::Vector2u footprint = b->GetFootprint();
                delete buildings[x][y];
                for(int i = 0; i < footprint.x; i++)
                    for(int j = 0; j < footprint.y; j++)
                        buildings[x + i][y - j] = 0;
            }
            else if(b)
                b->Reset();
        }
    }
    for(auto& vector : map)
        for(auto& tile : vector)
            tile->Reset();
}

Draggable* Level::GetBridge(unsigned int x, unsigned int y, int z)
{
    if(x < bridges.size())
    {
        if(y < bridges[x].size())
            return bridges[x][y].at(z);
        else
            return 0;
    }
    else
        return 0;
}

std::vector<Draggable*> Level::GetBridge(unsigned int x, unsigned int y)
{
    if(x < bridges.size())
    {
        if(y < bridges[x].size())
            return bridges[x][y];
        else
            return std::vector<Draggable*>();
    }
    else
        return std::vector<Draggable*>();
}

Building* Level::GetBuilding(unsigned int x, unsigned int y)
{
    if(x < buildings.size())
    {
        if(y < buildings[x].size())
		{
			if(buildings[x][y])
				return buildings[x][y];
			else
				return 0;
		}
        else
            return 0;
    }
    else
        return 0;
}

void Level::Terraform(int x, int y, int raise)
{
	bool valid = GetTile(x - 1, y + 1) && GetTile(x - 1, y) && GetTile(x, y) && GetTile(x, y + 1);
	if(valid)
	{
        GetTile(x, y)->Raise(raise, 1);
		GetTile(x, y + 1)->Raise(raise, 0);
		GetTile(x - 1, y + 1)->Raise(raise, 3);
		GetTile(x - 1, y)->Raise(raise, 2);
    }
}
unsigned int Level::GetShuffled(unsigned int i)
{
    return shuffledInts[i];
}
int Level::GetHeightForMouse(int x, int y)
{
	if(GetTile(x, y))
		return GetTile(x, y)->GetHeight()[1];
}

int Level::GradeBuilding(sf::IntRect bounds)
{
    //modify bounds to within map
    bounds.left = bounds.left < 0 ? 0 : bounds.left;
    bounds.top = bounds.top < 0 ? 0 : bounds.top;
    bounds.width = bounds.left +bounds.width < GetWidth() ? bounds.width : GetWidth() -bounds.left;
    bounds.height = bounds.top +bounds.height < GetWidth() ? bounds.height : GetWidth() -bounds.top;
    int plotavg = 0;
    for(int i = bounds.left; i < bounds.width; i++)
	{
		for(int j = bounds.top; j < bounds.height; j++)
		{
				int tileavg = (GetTile(i, j)->GetHeight()[0] +GetTile(i, j)->GetHeight()[1] +GetTile(i, j)->GetHeight()[2] +GetTile(i, j)->GetHeight()[3])/4;
				plotavg += tileavg;
		}
	}
	plotavg /= bounds.width * bounds.height;
	for(int i = bounds.left ; i < bounds.width +1; i++)
		for(int j = bounds.top ; j < bounds.height +1; j++)
            if(plotavg -(GetTile(i, j)->GetHeight()[1]))
                Terraform(i, j, plotavg -(GetTile(i, j)->GetHeight()[1]));
	return plotavg;
}
