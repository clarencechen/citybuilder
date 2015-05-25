#include "City.h"
#include "Level.h"
#include <iostream>
#include <math.h>
#include <set>
City::City()
{
    birthRate = 5.078125e-5;
    deathRate = 3.183594e-5;
    immigrationRate = 0.002f;
    emigrationRate = 0;
    propCanWork = 0.50f;
    populationPool = 1.0f;
    population = populationPool;
    employmentPool = 1.0f;
    employable = employmentPool;
    residentialTax = 0.10f;
    commercialTax = 0.10f;
    industrialTax = 0.10f;
    earnings = 0;
    funds = 0;
}

City::~City()
{

}

void City::Update(Level* level)
{
    alreadyDone.clear();
    double popTotal = 0;
    double commercialRevenue = 0;
    double industrialRevenue = 0;
    Building* z;
    /* Run first pass of tile updates. Mostly handles pool distribution. */
    for(int i = 0; i < (level->GetHeight()*level->GetWidth()); ++i)
    {
        z = level->GetBuilding(level->GetShuffled(i)/level->GetWidth(), level->GetShuffled(i)%level->GetWidth());
        if(!z || z->id == 1 || z->id >= 32 || z->GetPreview() || alreadyDone.count(z->GetAnchor().x*level->GetHeight() +z->GetAnchor().y))
            continue;
        unsigned int r = z->id;
        if(r % 3 == 1)//residential
        {
            /* Redistribute the pool and increase the population total by the tile's population */
            DistributePool(populationPool, z, birthRate -deathRate);
            popTotal += z->residents;
        }
        else if(r % 3 == 2)//commercial
        {
            /* Hire people. */
            if(rand() % 100 < 15 * (1.0 -commercialTax))
                DistributePool(employmentPool, z, 0.0f);
        }
        else if(r % 3 == 0)//industrial
        {
            /* Extract resources from the ground. */
            if(rand() % 100 < population*z->variant)
                ++z->production;
            /* Hire people. */
            if(rand() % 100 < 15 * (1.0 -industrialTax))
                DistributePool(employmentPool, z, 0.0f);
        }
        alreadyDone.insert(level->GetShuffled(z->GetAnchor().x*level->GetHeight() +z->GetAnchor().y));
    }
    alreadyDone.clear();
    /* Run second pass. Mostly handles goods manufacture */
    for(int i = 0; i < (level->GetHeight()*level->GetWidth()); ++i)
    {
        z = level->GetBuilding(level->GetShuffled(i)/level->GetWidth(), level->GetShuffled(i)%level->GetWidth());
        if(!z || z->id == 1 || z->id >= 32 || z->GetPreview() || alreadyDone.count(z->GetAnchor().x*level->GetHeight() +z->GetAnchor().y))
            continue;
        unsigned int r = z->id;
        if(r % 3 == 0)//industrial
        {
            int receivedResources = 0;
            /* Receive resources from smaller and connected zones */
            for(int i = 0; i < level->GetHeight(); i++)
            {
                for(int j = 0; j < level->GetWidth(); j++)
                {
                     Building* c = level->GetBuilding(i, j);
                     if(c && /*z->regions[0] == c->regions[0] &&*/ (c->id == 16 || c->id == 19 || c->id == 22))
                     {
                        if(c->production > 0)
                        {
                            ++receivedResources;
                            --c->production;
                        }
                        if(receivedResources > z->variant)
                            break;
                     }
                }
            }
            /* Turn resources into goods */
            if(z->storedGoods < z->variant*5)
            z->storedGoods += (receivedResources +z->production)*z->variant;
        }
        alreadyDone.insert(z->GetAnchor().x*level->GetHeight() +z->GetAnchor().y);
    }
    alreadyDone.clear();
    /* Run third pass. Mostly handles goods distribution. */
    for(int i = 0; i < (level->GetHeight()*level->GetWidth()); ++i)
    {
        z = level->GetBuilding(level->GetShuffled(i)/level->GetWidth(), (level->GetShuffled(i))%level->GetWidth());
        if(!z || z->id == 1 || z->id >= 32 || z->GetPreview() || alreadyDone.count(z->GetAnchor().x*level->GetHeight() +z->GetAnchor().y))
            continue;
        unsigned int r = z->id;
        if(r % 3 == 2)//commercial
        {
            int receivedGoods = 0;
            float maxCustomers = 0.0;
            for(int i = 0; i < level->GetHeight(); i++)
            {
                for(int j = 0; j < level->GetWidth(); j++)
                {
                    Building* c = level->GetBuilding(i, j);
                    if(c && /*z->regions[0] == c->regions[0] &&*/ (r % 3 == 0))
                        while(c->storedGoods > 0 && receivedGoods != z->variant)
                        {
                            --c->storedGoods;
                            ++receivedGoods;
                            industrialRevenue += 100 * (1.0 -industrialTax);
                        }
                    else if(c && /*z->regions[0] == c->regions[0] &&*/ (r % 3 == 1))
                         maxCustomers += c->residents;
                    if(receivedGoods == z->variant)
                        break;
                    /* Calculate the overall revenue for the tile. */
                    z->production = (receivedGoods*(100.0 + rand() % 20)) * (1.0- commercialTax);
                }
            }
            /* Calculate the overall revenue for the tile. */
            z->production = (receivedGoods*100.0 + rand() % 20) * (1.0-this->commercialTax);
            float revenue = z->production * maxCustomers * z->residents / 100.0;
            commercialRevenue += revenue;
        }
        alreadyDone.insert(z->GetAnchor().x*level->GetHeight() +z->GetAnchor().y);
    }
    alreadyDone.clear();
    /* Adjust population pool for births and deaths, and immigration. */
    populationPool += populationPool*(birthRate +immigrationRate -deathRate -emigrationRate);
    popTotal += populationPool;

    /* Adjust the employment pool for the changing population. */
    float newWorkers = (popTotal -population) * propCanWork;
    employmentPool += newWorkers;
    employable += newWorkers;

     if(employmentPool < 0)
        employmentPool = 0;
    if(employable < 0)
        employable = 0;

    /* Update the city population. */
    population = popTotal;

    /* Calculate city income from tax. */
    earnings = (population - populationPool) * 15 * residentialTax;
    earnings += commercialRevenue * commercialTax + industrialRevenue * industrialTax;
}

void City::Budget()
{
    std::cout << "Old City Funds: " << funds << std::endl;
    std::cout << "City Earnings: " << earnings << std::endl;
    funds += earnings;
    std::cout << "New City Funds: " << funds << std::endl;
}

void City::DistributePool(float& pool, Building* b, float rate)
{
    const static int moveRate = 64;
    unsigned int maxPop = b->variant ? (b->maxPopPerVariant)* pow(2, b->variant) : 0;
    /* If there is room in the b, move up to 64 people from the
     * pool into the b */
    if(pool > 0)
    {
        int moving = maxPop - b->residents;
        if(moving > moveRate)
            moving = moveRate;
        if(moving > pool)
            moving = pool;
        pool -= moving;
        b->residents += moving;
    }
    /* Adjust the b population for births and deaths */
    b->residents += b->residents * rate;
    /* Move population that cannot be sustained by the b into
     * the pool */
    if(b->residents > maxPop)
    {
        pool += b->residents - maxPop;
        b->residents = maxPop;
        immigrationRate = fabs(immigrationRate -b->variant*0.00001f);
    }
    // Construct new zones if there is enough demand
    if(pool > b->maxPopPerVariant * pow(2, b->variant +1) && b->variant < b->maxVariants)
        if(rand() % 1000 < 100.0 / (b->variant +1))
        {
            //++b->variant;
            b->variant += 4;
            immigrationRate += b->variant*0.00001f;
        }
    b->SetStatus(b->GetDisplayTile(0), false);
}

double City::GetHomeless()
{
    return populationPool;
}
double City::GetUnemployed()
{
    return employmentPool;
}
void City::DumpStats()
{
    std::cout << populationPool << " homeless out of " << population << " total." << std::endl;
    std::cout << employmentPool << " unemployed out of " << employable << " total." << std::endl;
    std::cout << immigrationRate << " immgration rate and " << emigrationRate << " emigration rate." << std::endl;
    std::cout << "Taxes: " << residentialTax << ", " << commercialTax << ", " << industrialTax << std::endl;
}
