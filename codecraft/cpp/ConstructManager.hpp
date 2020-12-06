#ifndef _CONSTRUCT_HPP_
#define _CONSTRUCT_HPP_

#include "Square.hpp"
#include <unordered_map>
#include <map>
#include "model/Model.hpp"
#include "Economy.hpp"
#include "BuilderManager.hpp"
#include "ArmyManager.hpp"
#include <vector>
#include <utility>

class ConstructManager {
public:
    ConstructManager();
    void updateBases(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases);
    void baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, const BuilderManager & builderManager, const ArmyManager & armyManager, std::vector<std::vector<Square> > & open);
    void updateHouseBuilds(BuilderManager & builderManager, std::vector<std::vector<Square> > & open);
    void initHouseLocations();

private:
    std::unordered_map<int, Entity> builderFactories;
    std::unordered_map<int, Entity> rangedFactories;
    std::unordered_map<int, Entity> meleeFactories;
    std::map<Vec2Int, int> houseLocations;
};

#endif