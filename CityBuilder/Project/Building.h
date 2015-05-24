#ifndef BUILDING_H
#define BUILDING_H

#include <SFML\Graphics.hpp>
class Level;
#include "Level.h"
enum class Type {STRUCTURE, ZONE, DRAGGABLE};
class Building
{
friend class City;
protected:
	int x;
	int y;
	int z[4];
	sf::Vector2u buffer;
	unsigned int baseid;
    unsigned int transform;
    bool preview;
    bool condemn;
    bool stick;
	sf::Texture* texture;
    unsigned int idbuffer;
	unsigned int id;
    unsigned int variant;
    float residents;
    unsigned int maxPopPerVariant;
    unsigned int maxVariants;
    float production;
    float storedGoods;
public:

	Type type;
	static const int tilesize = 32;
	Building(int x, int y, int z, bool preview);
	Building(int x, int y, int z[4], bool preview);
	~Building();
	void Draw(sf::Vector2i camOffset, sf::RenderWindow* rw, ImageManager& imagemanager);
	void SetStatus(sf::Vector2u status, bool preview);
	bool GetDel();
	bool GetPreview();
	sf::Vector3f GetPop();
    unsigned int GetId();
	void Condemn();
	sf::Vector2u GetAnchor();
	bool operator==(const Building &other);
	virtual void Reset();
	virtual unsigned int GetRoad();
	virtual unsigned int GetRail();
	virtual void Add(unsigned int id, bool preview);
	virtual sf::Vector2u GetFootprint() = 0;
	virtual sf::Vector2u GetDisplayTile(Level* level) = 0;
	virtual void MatchNetwork(bool preview, Level* level);
};
#include "Transport.h"
#include "Structure.h"
#include "Zone.h"
#endif // BUILDING_H
