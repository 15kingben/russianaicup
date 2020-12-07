#ifndef BUILDING_HPP
#define BUILDING_HPP

#include "model/Model.hpp"
#include <vector>
#include "Repairable.hpp"

class Building : public Repairable {
    public:
        Building();
        Building(Entity entity);
        Entity entity;
        Entity getEntity();
};

#endif