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
    bool charge(EntityType entityType);
    void updateResourcePerTurn();
    void updatePopulation(Entity entity);
    void setPopulation(int pop);
    int getPopulation();
    int getUsedPopulation();
private:
    int resourcePerTurn;
    int resources;
    int population;
    int usedPopulation;
};

#endif