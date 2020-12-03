#ifndef _CONSTRUCT_HPP_
#define _CONSTRUCT_HPP_

#include <unordered_map>
#include "model/Model.hpp"
#include "Economy.hpp"
#include "BuilderManager.hpp"
#include "ArmyManager.hpp"

class ConstructManager {
public:
    ConstructManager();
    void updateBases(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases);
    void baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, const BuilderManager & builderManager, const ArmyManager & armyManager);

private:
    std::unordered_map<int, Entity> builderFactories;
    std::unordered_map<int, Entity> rangedFactories;
    std::unordered_map<int, Entity> meleeFactories;
};

#endif