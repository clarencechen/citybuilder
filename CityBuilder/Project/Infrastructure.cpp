#include "Level.h"
#include "Engine.h"
#include "Tile.h"
#include <SFML\Graphics.hpp>
#include <iostream>

void Level::Place(int x, int y, int z, bool stick, unsigned int r, bool preview, ImageManager& imageManager)
{
    //catch for out of bounds
    if (x < 0 || y < 0 || x >= w || y >= h)
        return;
    else
    {
		Building* building = GetBuilding(x, y);
		Building* bridge = GetBridge(x, y, z);
		if(r == 0) //delete tool
		{
			if(preview && !stick && bridge)
				bridge->Condemn();
			else if (preview && stick && building)
				building->Condemn();
			else if(!preview && !stick && bridge)
			{
				delete bridge;
				bridge = 0;
			}
            else if(!preview && stick && building)
			{
				int id = building->id;
				delete building;
					for(int i = 0; i < footprint[id].x; i++)
						for(int j = 0; j < footprint[id].y; j++)
							buildings[x + i][y - j] = 0;
			}
			for(int i = x - 1; i <= x + 1; i++)
            {
                for(int j = y - 1; j <= y + 1; j++)
                {
                    if(stick ? GetBuilding(i, j) : GetBridge(i, j, z))
                    {
						MatchRoad(i, j, z, stick);
						building = stick ? GetBuilding(i, j) : GetBridge(i, j, z);
						building->SetStatus(GetBuildingTile(i, j, z, stick), preview, imageManager);
                    }
                }
            }
		}
		else if(r <= 16)
        {
			if(!stick && !bridge)
				bridges[x][y][z] = new Building(r, x, y, z, preview);
			else if(stick && !building)
				buildings[x][y] = new Building(r, x, y, GetTile(x, y)->GetHeight(), preview);
			else if(!stick && bridge)
				bridge->SetStatus(r, preview);
            else if(stick && building)
				//to make sure roads don't collide into buildings
				if(building-> id > 16 && r)
					return;
				else
					building->SetStatus(r, preview);
            //Update display tiles
            for(int i = x - 1; i <= x + 1; i++)
            {
                for(int j = y - 1; j <= y + 1; j++)
                {
                    if(stick ? GetBuilding(i, j) : GetBridge(i, j, z))
                    {
						MatchRoad(i, j, z, stick);
						Building* neighbors = stick ? GetBuilding(i, j) : GetBridge(i, j, z);
						neighbors->SetStatus(GetBuildingTile(i, j, z, stick), preview, imageManager);
                    }
                }
            }
        }
        else if(r < 128)
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
			buildings[x][y] = new Building(r, x, y, GetTile(x, y)->GetHeight(), preview);
			for(int i = 0; i < footprint[r].x; i++)
				for(int j = 0; j < footprint[r].y; j++)
					buildings[x + i][y - j] = buildings[x][y];
			GetBuilding(x, y)->SetStatus(GetBuildingTile(x, y, z, stick), preview, imageManager);
        }
    }
}

void Level::Reset(ImageManager& imageManager)
{
    for(auto& vector : bridges)
    {
        for(auto& vec : vector)
        {
            for(Building*& bridge : vec)
            {
                if(bridge)
                {
                    if(bridge->GetDel())
                    {
                        delete bridge;
                        bridge = 0;
                    }
                    else
                        bridge->Reset(imageManager);
                }
            }
        }
    }
    for(int x = 0; x < w; ++x)
    {
        for(int y = h - 1; y >= 0; --y)
        {
            if(GetBuilding(x, y))
			{
				if(GetBuilding(x, y)->GetDel())
                {
                    int r = GetBuilding(x, y)->id;
                    delete buildings[x][y];
					for(int i = 0; i < footprint[r].x; i++)
						for(int j = 0; j < footprint[r].y; j++)
							buildings[x + i][y - j] = 0;
				}
                else
                    GetBuilding(x, y)->Reset(imageManager);
			}
        }
    }
    for(auto& vector : map)
    {
        for(auto& tile : vector)
        {
            tile->Reset();
        }
    }
}

void Level::MatchRoad(int x, int y, int z, bool stick)
{
    int id = GetId(x, y, z, stick);
    int nb[4];
    nb[0] = GetNeighbors(x, y, z, stick)[0];
    nb[1] = GetNeighbors(x, y, z, stick)[1];
    nb[2] = GetNeighbors(x, y, z, stick)[2];
    nb[3] = GetNeighbors(x, y, z, stick)[3];
    if(id == 1)
    {
        bool one = (nb[0] != 2 && nb[1] != 2 && nb[2] != 2 && nb[3] != 2);
        for(int i = 0; i < 4; i++)
        {
            if(nb[i] == 2 &&
                    nb[(i + 1) % 4] != 1 && nb[(i + 1) % 4] != 2 &&
                    nb[(i + 3) % 4] != 1 && nb[(i + 3) % 4] != 2 &&
                    nb[(i + 2) % 4] != 2)
                one = true;
        }
        if(one)
            stick ? GetBuilding(x, y)->id = 1 : GetBridge(x, y, z)->id = 1;
        else
            stick ? GetBuilding(x, y)->id = 2 : GetBridge(x, y, z)->id = 2;
    }
    else if(id == 2)
    {
        bool one = (nb[0] != 2 && nb[1] != 2 && nb[2] != 2 && nb[3] != 2);
        if(!one || (nb[0] > 2 && nb[1] > 2 && nb[2] > 2 && nb[3] > 2) || (nb[0] == 0 && nb[1] == 0 && nb[2] == 0 && nb[3] == 0))
			stick ? GetBuilding(x, y)->id = 2 : GetBridge(x, y, z)->id = 2;
        else
            stick ? GetBuilding(x, y)->id = 1 : GetBridge(x, y, z)->id = 1;
    }
}

Building* Level::GetBridge(unsigned int x, unsigned int y, int z)
{
    if(x < bridges.size())
    {
        if(y < bridges[x].size())
        {
            return bridges[x][y].at(z);
        }
        else
            return 0;
    }
    else
        return 0;
}

std::vector<Building*> Level::GetBridge(unsigned int x, unsigned int y)
{
    if(x < bridges.size())
    {
        if(y < bridges[x].size())
        {
            return bridges[x][y];
        }
        else
        {
            return std::vector<Building*>();
        }
    }
    else
        return std::vector<Building*>();
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

unsigned int Level::GetId(unsigned int x, unsigned int y, int z, bool stick)
{
	return stick ? (GetBuilding(x, y) ? GetBuilding(x, y)->id : 0) : (GetBridge(x, y, z) ? GetBridge(x, y, z)->id : 0);
}

sf::Vector2u Level::GetBuildingTile(unsigned int x, unsigned int y, int z, bool stick)
{
    if((stick && !GetBuilding(x, y)) || (!stick && !GetBridge(x, y, z)))
        return sf::Vector2u(0, 0);
    int nb[4];
    nb[0] = GetNeighbors(x, y, z, stick)[0];
    nb[1] = GetNeighbors(x, y, z, stick)[1];
    nb[2] = GetNeighbors(x, y, z, stick)[2];
    nb[3] = GetNeighbors(x, y, z, stick)[3];
    int id = GetId(x, y, z, stick);
    switch (id)
    {
    case 0 :
        return sf::Vector2u(0, 0);
    case 1 ://small road
    {
        bool bnb [4] =
        {
            nb[0] == 1 || nb[0] == 2,
            nb[1] == 1 || nb[1] == 2,
            nb[2] == 1 || nb[2] == 2,
            nb[3] == 1 || nb[3] == 2
        };
        //return 40
        if(bnb[0] && bnb[1] && bnb[2] && bnb[3])
            return sf::Vector2u(40, 0);
        for(int i = 0; i < 4; i++)
        {
            //return 30
            if(!bnb[i] && bnb[(i + 1) % 4] && bnb[(i + 2) % 4] && bnb[(i + 3) % 4])
                return sf::Vector2u(30, i);
            //return 23
            if(!bnb[i] && !bnb[(i + 1) % 4] && bnb[(i + 2) % 4] && bnb[(i + 3) % 4])
                return sf::Vector2u(23, i);
            //return 20
            if(bnb[i] && !bnb[(i + 1) % 4] && bnb[(i + 2) % 4] && !bnb[(i + 3) % 4])
                return sf::Vector2u(20, i);
            //return 11
            if(!bnb[i] && !bnb[(i + 1) % 4] && bnb[(i + 2) % 4] && !bnb[(i + 3) % 4])
                return sf::Vector2u(11, i);
        }
        //return 10
        return sf::Vector2u(10, 0);
    }
    case 2 ://large road
    {
        //return 41
        if(nb[0] == 2 && nb[1] == 2 && nb[2] == 2 && nb[3] == 2)
            return sf::Vector2u(41, 0);

        for(int i = 0; i < 4; i++)
        {
            //return 31
            if((nb[i] == 0 || nb[i] > 2) && nb[(i + 1) % 4] == 2 && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 2)
                return sf::Vector2u(31, i);
            //return 42
            if(nb[i] == 2 && nb[(i + 1) % 4] == 1 && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 1)
                return sf::Vector2u(42, i);
            //return 32
            if((nb[i] == 0 || nb[i] > 2) && nb[(i + 1) % 4] == 2 && nb[(i + 2) % 4] == 1 && nb[(i + 3) % 4] == 2)
                return sf::Vector2u(32, i);
            //return 33
            if((nb[i] == 0 || nb[i] > 2) && nb[(i + 1) % 4] == 1 && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 1)
                return sf::Vector2u(33, i);
            //return 43
            if(nb[i] == 2 && nb[(i + 1) % 4] == 2 && nb[(i + 2) % 4] == 1 && nb[(i + 3) % 4] == 1)
                return sf::Vector2u(43, i);
            //return 44
            if(nb[i] == 1 && nb[(i + 1) % 4] == 2 && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 2)
                return sf::Vector2u(44, i);
            //return 45
            if(nb[i] == 2 && nb[(i + 1) % 4] == 1 && nb[(i + 2) % 4] == 1 && nb[(i + 3) % 4] == 1)
                return sf::Vector2u(45, i);
            //return 34
            if((nb[i] == 0 || nb[i] > 2) && nb[(i + 1) % 4] == 1 && nb[(i + 2) % 4] == 1 && nb[(i + 3) % 4] == 2)
                return sf::Vector2u(34, i);
            //return 35
            if((nb[i] == 0 || nb[i] > 2) && nb[(i + 1) % 4] == 2 && nb[(i + 2) % 4] == 1 && nb[(i + 3) % 4] == 1)
                return sf::Vector2u(35, i);
            //return 36
            if((nb[i] == 0 || nb[i] > 2) && nb[(i + 1) % 4] == 2 && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 1)
                return sf::Vector2u(36, i);
            //return 37
            if((nb[i] == 0 || nb[i] > 2) && nb[(i + 1) % 4] == 1 && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 2)
                return sf::Vector2u(37, i);
            //return 24
            if((nb[i] == 0 || nb[i] > 2) && (nb[(i + 1) % 4] == 0 || nb[(i + 1) % 4] > 2) && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 2)
                return sf::Vector2u(24, i);
            //return 25
            if((nb[i] == 0 || nb[i] > 2) && (nb[(i + 1) % 4] == 0 || nb[(i + 1) % 4] > 2) && nb[(i + 2) % 4] == 1 && nb[(i + 3) % 4] == 2)
                return sf::Vector2u(25, i);
            //return 26
            if((nb[i] == 0 || nb[i] > 2) && (nb[(i + 1) % 4] == 0 || nb[(i + 1) % 4] > 2) && nb[(i + 2) % 4] == 2 && nb[(i + 3) % 4] == 1)
                return sf::Vector2u(26, i);
            //return 21
            if(nb[i] == 2 && (nb[(i + 1) % 4] == 0 || nb[(i + 1) % 4] > 2) && nb[(i + 2) % 4] == 2 && (nb[(i + 3) % 4] == 0 || nb[(i + 3) % 4] > 2))
                return sf::Vector2u(21, i);
            //return 22
            if(nb[i] == 2 && (nb[(i + 1) % 4] == 0 || nb[(i + 1) % 4] > 2) && nb[(i + 2) % 4] == 1 && (nb[(i + 3) % 4] == 0 || nb[(i + 3) % 4] > 2))
                return sf::Vector2u(22, i);
            //return 12
            if((nb[i] == 0 || nb[i] > 2) && (nb[(i + 1) % 4] == 0 || nb[(i + 1) % 4] > 2) && (nb[(i + 2) % 4] == 2 || nb[(i + 2) % 4] == 1) && (nb[(i + 3) % 4] == 0 || nb[(i + 3) % 4] > 2))
                return sf::Vector2u(12, i);
        }
        //return 10
        return sf::Vector2u(10, 0);
    }
    default:
    {
        return sf::Vector2u(id + 96, 0);
    }
    }
}
int(&Level::GetNeighbors(unsigned int x, unsigned int y, int z, bool stick))[4]
{
	int nb[4] = {GetId(x, y - 1, z, stick), GetId(x + 1, y, z, stick), GetId(x, y + 1, z, stick), GetId(x - 1, y, z, stick)};
	return nb;
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

int Level::GetHeightForMouse(int x, int y)
{
	if(GetTile(x, y))
		return GetTile(x, y)->GetHeight()[1];
}

void Level::GradeBuilding(sf::IntRect bounds)
{
    int plotavg = 0;
    for(int i = bounds.left; i < bounds.width; i++)
	{
		for(int j = bounds.top; j < bounds.height; j++)
		{
			if(GetTile(i, j) != 0)
			{
				int tileavg = (GetTile(i, j)->GetHeight()[0] +GetTile(i, j)->GetHeight()[1] +GetTile(i, j)->GetHeight()[2] +GetTile(i, j)->GetHeight()[3])/4;
				for (int k = 0; k < 4; k++)
					GetTile(i, j)->SetHeight(tileavg, k);
				plotavg += tileavg;
			}
		}
	}
	plotavg /= bounds.width * bounds.height;
	for(int i = bounds.left; i < bounds.width; i++)
	{
		for(int j = bounds.top; j < bounds.height; j++)
		{
			if(GetTile(i, j) != 0)
			{
				for (int k = 0; k < 4; k++)
					GetTile(i, j)->SetHeight(plotavg, k);
			}
		}
	}
}
