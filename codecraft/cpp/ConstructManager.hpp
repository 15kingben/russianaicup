#ifndef _CONSTRUCT_HPP_
#define _CONSTRUCT_HPP_

#include <unordered_map>
#include "model/Model.hpp"
#include "Economy.hpp"
#include "BuilderManager.hpp"
#include "ArmyManager.hpp"
#include <vector>

class Square {
    public:
        Square();
        Square(Entity entity);
        void setEntity(Entity entity);
        bool isOccupied();
        void setPlanned();
        Entity getEntity();
    private:
        bool occupied;
        Entity entity;
};

class ConstructManager {
public:
    ConstructManager();
    void updateBases(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases);
    void baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, const BuilderManager & builderManager, const ArmyManager & armyManager, std::vector<std::vector<Square> > & open);

private:
    std::unordered_map<int, Entity> builderFactories;
    std::unordered_map<int, Entity> rangedFactories;
    std::unordered_map<int, Entity> meleeFactories;
};

#endif