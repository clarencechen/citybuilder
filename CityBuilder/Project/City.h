#ifndef CITY_H
#define CITY_H
#include "Structure.h"
#include <set>
class Structure;
class City
{
protected:
    std::set<int> alreadyDone;
    /* Number of residents who are not in a residential zone. */
    float populationPool;

    /* Number of residents who are not currently employed but can work. */
    float employmentPool;

    /* Proportion of citizens who can work. */
    float propCanWork;

    /* Proportion of residents who die/give birth each day.
     * Estimate for death rate = 1 / (life expectancy * 360)
     * Current world values are 0.000055 and 0.000023, respectively */
    float birthRate;
    float deathRate;
    float immigrationRate;
    float emigrationRate;
    void DistributePool(float& pool, Building* b, float rate);
public:
    float population;
    float employable;

    float residentialTax;
    float commercialTax;
    float industrialTax;

    /* Running total of city earnings (from tax etc) this month. */
    float earnings;
    float funds;
    City();
    ~City();
    void Update(Level* level);
    void Budget();
    double GetHomeless();
    double GetUnemployed();
    void DumpStats();
};

#endif // CITY_H
