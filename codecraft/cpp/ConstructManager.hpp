#ifndef _CONSTRUCT_HPP_
#define _CONSTRUCT_HPP_

#include "Square.hpp"
#include "BuilderManager.hpp"
#include <unordered_map>
#include <map>
#include "model/Model.hpp"
#include "Economy.hpp"
#include "ArmyManager.hpp"
#include <vector>
#include <utility>
#include "Building.hpp"

class ConstructManager {
public:
    ConstructManager();
    void updateBuildings(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases,
                    const std::unordered_map<int, Entity> & houses,
                    const std::unordered_map<int, Entity> & turrets);

    void baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, BuilderManager & builderManager, const ArmyManager & armyManager, std::vector<std::vector<Square> > & open);
    void updateHouseBuilds(BuilderManager & builderManager, std::vector<std::vector<Square> > & open);
    void repairBuildings(BuilderManager & builderManager, std::vector<std::vector<Square> > & open);
    void initHouseLocations();

private:
    std::unordered_map<int, Building> builderFactories;
    std::unordered_map<int, Building> rangedFactories;
    std::unordered_map<int, Building> meleeFactories;
    std::unordered_map<int, Building> houses;
    std::unordered_map<int, Building> turrets;
    std::map<std::pair<int, int>, int> houseLocations;
};

#endif