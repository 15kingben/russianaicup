#ifndef _ECONOMY_HPP_
#define _ECONOMY_HPP_

#include "model/Model.hpp"

class Economy {
public:
    Economy();
    int getResources();
    int getResourcePerTurn();
    void setResources(int resource);
    bool charge(EntityType entityType, int cost);
    void updateResourcePerTurn();
    void updatePopulation(EntityType entityType);
    void setPopulation(int pop);
    int getPopulation();
private:
    int resourcePerTurn;
    int resources;
    int population;
};

#endif