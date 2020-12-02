#ifndef _ECONOMY_HPP_
#define _ECONOMY_HPP_

#include "model/Model.hpp"

class Economy {
public:
    Economy();
    int getResources();
    int getResourcePerTurn();
    void setResources(int resource);
    bool charge(EntityType entityType);
    void updateResourcePerTurn();
private:
    int resourcePerTurn;
    int resources;
};

#endif