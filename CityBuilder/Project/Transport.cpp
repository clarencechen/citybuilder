#include <SFML\Graphics.hpp>
#include <math.h>
#include "Level.h"
#include "Building.h"
#include "ImageManager.h"
#include "Infrastructure.h"
#include "TerrainTile.h"
#include "Transport.h"

Draggable::Draggable(unsigned int id, int x, int y, int z[4], bool preview, Level* level) : Building(x, y, z, preview)
{

	stick = true;
	neighbors[0] = level->GetBuilding(x, y - 1);
	neighbors[1] = level->GetBuilding(x + 1, y);
	neighbors[2] = level->GetBuilding(x, y + 1);
	neighbors[3] = level->GetBuilding(x - 1, y);
	switch (id)
	{
	case 1:
	{
		railState = 0;
		roadState = 1;
		railBuffer = 0;
		roadBuffer = preview ? 0 : 1;
		break;
	}
	case 2:
	{
		railState = 0;
		roadState = 2;
		railBuffer = 0;
		roadBuffer = preview ? 0 : 2;
		break;
	}
	case 3:
	{
		railState = 1;
		roadState = 0;
		railBuffer = preview ? 0 : 1;
		roadBuffer = 0;
		break;
	}
	default:
	{
		break;
	}
	}
}

Draggable::Draggable(unsigned int id, int x, int y, int z, bool preview, Level* level) : Building(x, y, z, preview)
{
	stick = false;
	neighbors[0] = level->GetBridge(x, y - 1, z);
	neighbors[1] = level->GetBridge(x + 1, y, z);
	neighbors[2] = level->GetBridge(x, y + 1, z);
	neighbors[3] = level->GetBridge(x - 1, y, z);
	switch (id)
	{
	case 1:
	{
		railState = 0;
		roadState = 1;
		railBuffer = 0;
		roadBuffer = preview ? 0 : 1;
		break;
	}
	case 2:
	{
		railState = 0;
		roadState = 2;
		railBuffer = 0;
		roadBuffer = preview ? 0 : 2;
		break;
	}
	case 3:
	{
		railState = 1;
		roadState = 0;
		railBuffer = preview ? 0 : 1;
		roadBuffer = 0;
		break;
	}
	default:
		break;
	}
}
unsigned int Draggable::GetRoad()
{
	return roadState;
}

unsigned int Draggable::GetRail()
{
	return railState;
}

void Draggable::SetRoad(unsigned int r, bool preview)
{
	roadState = r;
	if(!preview)
		roadBuffer = r;
}

void Draggable::SetRail(unsigned int r, bool preview)
{
	railState = r;
	if(!preview)
		railBuffer = r;
}

void Draggable::Reset(ImageManager& imageManager)
{
	Building::Reset(imageManager);
	roadState = roadBuffer;
	railState = railBuffer;
}

void Draggable::Add(unsigned int id, bool preview)
{
	this->preview = preview;
	switch (id)
	{
	case 1:
		roadState = 1;
		if(!preview)
			roadBuffer = 1;
		break;
	case 2:
		roadState = 2;
		if(!preview)
			roadBuffer = 2;
		break;
	case 3:
		railState = 1;
		if(!preview)
			railBuffer = 1;
		break;
//	case 4:
//		if(railState == 1 && roadState == 0)
//			railState = 2;
//		if(!preview && railState == 1 && roadBuffer == 0)
//			railBuffer = 2;
//		break;
	}
}

void Draggable::MatchNetwork(bool preview, Level* level)
{
	neighbors[0] = stick ? level->GetBuilding(x, y - 1) : level->GetBridge(x, y - 1, z[0]);
	neighbors[1] = stick ? level->GetBuilding(x + 1, y) : level->GetBridge(x + 1, y, z[0]);
	neighbors[2] = stick ? level->GetBuilding(x, y + 1) : level->GetBridge(x, y + 1, z[0]);
	neighbors[3] = stick ? level->GetBuilding(x - 1, y) : level->GetBridge(x - 1, y, z[0]);
    unsigned int road[4] = {neighbors[0] ? neighbors[0]->GetRoad() : 0,
							neighbors[1] ? neighbors[1]->GetRoad() : 0,
							neighbors[2] ? neighbors[2]->GetRoad() : 0,
							neighbors[3] ? neighbors[3]->GetRoad() : 0};
	unsigned int rail[4] = {neighbors[0] ? neighbors[0]->GetRail() : 0,
							neighbors[1] ? neighbors[1]->GetRail() : 0,
							neighbors[2] ? neighbors[2]->GetRail() : 0,
							neighbors[3] ? neighbors[3]->GetRail() : 0};
    if(roadState == 1 && railState == 0)
    {
        bool one = (road[0] != 2 && road[1] != 2 && road[2] != 2 && road[3] != 2);
        for(int i = 0; i < 4; i++)
        {
            if(road[i] == 2 &&
                    road[(i + 1) % 4] != 1 && road[(i + 1) % 4] != 2 &&
                    road[(i + 3) % 4] != 1 && road[(i + 3) % 4] != 2 &&
                    road[(i + 2) % 4] != 2)
                one = true;
        }
        if(!one)
            SetRoad(2, preview);
    }
    else if(roadState == 2 && railState == 0)
    {
        bool one = (road[0] != 2 && road[1] != 2 && road[2] != 2 && road[3] != 2);
        if(one && road[0] && road[1] && road[2] && road[3])
			SetRoad(1, preview);
    }
    else if(roadState && railState == 1)
	{
		if(road[0] && road[2])
			if(!(rail[1] && rail[3]))
				SetRail(0, preview);
			else
				return;
		else if(road[3] && road[1])
			if(!(rail[0] && rail[2]))
				SetRail(0, preview);
			else
				return;
		else if((rail[0] && rail[2]) || (rail[1] && rail[3]))
			SetRoad(0, preview);
	}
//	else if(railState == 2)
//	{
//		if(!(rail[0] && !rail[1] && rail[2] && !rail[3]) && !(!rail[0] && rail[1] && !rail[2] && rail[3]))
//			SetRail(1, preview);
//	}
}


sf::Vector2u Draggable::GetDisplayTile(Level* level)
{
 	neighbors[0] = stick ? level->GetBuilding(x, y - 1) : level->GetBridge(x, y - 1, z[0]);
	neighbors[1] = stick ? level->GetBuilding(x + 1, y) : level->GetBridge(x + 1, y, z[0]);
	neighbors[2] = stick ? level->GetBuilding(x, y + 1) : level->GetBridge(x, y + 1, z[0]);
	neighbors[3] = stick ? level->GetBuilding(x - 1, y) : level->GetBridge(x - 1, y, z[0]);
    unsigned int road[4] = {neighbors[0] ? neighbors[0]->GetRoad() : 0,
							neighbors[1] ? neighbors[1]->GetRoad() : 0,
							neighbors[2] ? neighbors[2]->GetRoad() : 0,
							neighbors[3] ? neighbors[3]->GetRoad() : 0};
	unsigned int rail[4] = {neighbors[0] ? neighbors[0]->GetRail() : 0,
							neighbors[1] ? neighbors[1]->GetRail() : 0,
							neighbors[2] ? neighbors[2]->GetRail() : 0,
							neighbors[3] ? neighbors[3]->GetRail() : 0};
	if(railState == 0)
	{
		switch (roadState)
		{
		case 0 :
			return sf::Vector2u(0, 0);
		case 1 ://small road
		{
			bool bnb [4] =
			{
				road[0] == 1 || road[0] == 2,
				road[1] == 1 || road[1] == 2,
				road[2] == 1 || road[2] == 2,
				road[3] == 1 || road[3] == 2
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
			if(road[0] == road[1] == road[2] == road[3] == 2)
				return sf::Vector2u(41, 0);

			for(int i = 0; i < 4; i++)
			{
				//return 31
				if(road[i] == 0 && road[(i + 1) % 4] == 2 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 2)
					return sf::Vector2u(31, i);
				//return 42
				if(road[i] == 2 && road[(i + 1) % 4] == 1 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 1)
					return sf::Vector2u(42, i);
				//return 32
				if(road[i] == 0 && road[(i + 1) % 4] == 2 && road[(i + 2) % 4] == 1 && road[(i + 3) % 4] == 2)
					return sf::Vector2u(32, i);
				//return 33
				if(road[i] == 0 && road[(i + 1) % 4] == 1 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 1)
					return sf::Vector2u(33, i);
				//return 43
				if(road[i] == 2 && road[(i + 1) % 4] == 2 && road[(i + 2) % 4] == 1 && road[(i + 3) % 4] == 1)
					return sf::Vector2u(43, i);
				//return 44
				if(road[i] == 1 && road[(i + 1) % 4] == 2 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 2)
					return sf::Vector2u(44, i);
				//return 45
				if(road[i] == 2 && road[(i + 1) % 4] == 1 && road[(i + 2) % 4] == 1 && road[(i + 3) % 4] == 1)
					return sf::Vector2u(45, i);
				//return 34
				if(road[i] == 0 && road[(i + 1) % 4] == 1 && road[(i + 2) % 4] == 1 && road[(i + 3) % 4] == 2)
					return sf::Vector2u(34, i);
				//return 35
				if(road[i] == 0 && road[(i + 1) % 4] == 2 && road[(i + 2) % 4] == 1 && road[(i + 3) % 4] == 1)
					return sf::Vector2u(35, i);
				//return 36
				if(road[i] == 0 && road[(i + 1) % 4] == 2 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 1)
					return sf::Vector2u(36, i);
				//return 37
				if(road[i] == 0 && road[(i + 1) % 4] == 1 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 2)
					return sf::Vector2u(37, i);
				//return 24
				if(road[i] == 0 && road[(i + 1) % 4] == 0 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 2)
					return sf::Vector2u(24, i);
				//return 25
				if(road[i] == 0 && road[(i + 1) % 4] == 0 && road[(i + 2) % 4] == 1 && road[(i + 3) % 4] == 2)
					return sf::Vector2u(25, i);
				//return 26
				if(road[i] == 0 && road[(i + 1) % 4] == 0 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 1)
					return sf::Vector2u(26, i);
				//return 21
				if(road[i] == 2 && road[(i + 1) % 4] == 0 && road[(i + 2) % 4] == 2 && road[(i + 3) % 4] == 0)
					return sf::Vector2u(21, i);
				//return 22
				if(road[i] == 2 && road[(i + 1) % 4] == 0 && road[(i + 2) % 4] == 1 && road[(i + 3) % 4] == 0)
					return sf::Vector2u(22, i);
				//return 12
				if(road[i] == 0 && road[(i + 1) % 4] == 0 && (road[(i + 2) % 4] == 2 || road[(i + 2) % 4] == 1) && road[(i + 3) % 4] == 0)
					return sf::Vector2u(12, i);
			}
			//return 10
			return sf::Vector2u(10, 0);
		}
		}
	}
	else if(railState == 1)
	{
		switch(roadState)
		{
		case 0:
			if(rail[0] && rail[1] && rail[2] && rail[3])
				return sf::Vector2u(55, 0);
			for(int i = 0; i < 4; i++)
			{
				//return 54
				if(!rail[i] && rail[(i + 1) % 4] && rail[(i + 2) % 4] && rail[(i + 3) % 4])
					return sf::Vector2u(54, i);
				//return 53
				if(!rail[i] && !rail[(i + 1) % 4] && rail[(i + 2) % 4] && rail[(i + 3) % 4])
					return sf::Vector2u(53, i);
				//return 52
				if(rail[i] && !rail[(i + 1) % 4] && rail[(i + 2) % 4] && !rail[(i + 3) % 4])
					return sf::Vector2u(52, i);
				//return 51
				if(!rail[i] && !rail[(i + 1) % 4] && rail[(i + 2) % 4] && !rail[(i + 3) % 4])
					return sf::Vector2u(51, i);
			}
			return sf::Vector2u(50, 0);
		case 1:
			for(int i = 0; i < 4; i++)
				if(rail[i] && !rail[(i + 1) % 4] && rail[(i + 2) % 4] && !rail[(i + 3) % 4])
					return sf::Vector2u(60, i);
			return sf::Vector2u(1, 0);
		case 2:
			for(int i = 0; i < 4; i++)
				if(rail[i] && !rail[(i + 1) % 4] && rail[(i + 2) % 4] && !rail[(i + 3) % 4])
					return sf::Vector2u(61, i);
			return sf::Vector2u(1, 0);
		}
	}
//	else if(railState == 2)
//	{
//		for(int i = 0; i < 4; i++)
//		{
//			if(rail[i] == 1 && !rail[(i + 1) % 4] && rail[(i + 2) % 4] == 1 && !rail[(i + 3) % 4])
//				return sf::Vector2u(59, i);
//			if(rail[i] == 1 && !rail[(i + 1) % 4] && rail[(i + 2) % 4] == 2 && !rail[(i + 3) % 4])
//				return sf::Vector2u(58, i);
//			if(rail[i] == 2 && rail[(i + 2) % 4] == 2)
//				return sf::Vector2u(57, i);
//		}
//	}
}

sf::Vector2u Draggable::GetFootprint()
{
	return sf::Vector2u(1, 1);
}
