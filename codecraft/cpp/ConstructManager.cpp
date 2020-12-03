#include "ConstructManager.hpp"
#include "BuilderManager.hpp"
#include "ArmyManager.hpp"
#include "Util.hpp"
#include "Economy.hpp"
#include <iostream>

ConstructManager::ConstructManager() {

}

 void ConstructManager::updateBases(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases) {
    meleeFactories.clear();
    for (auto pair : meleeBases) {
        meleeFactories[pair.first] = pair.second;
    }

    builderFactories.clear();
    for (auto pair : builderBases) {
        builderFactories[pair.first] = pair.second;
    }

    rangedFactories.clear();
    for (auto pair : rangedBases) {
        rangedFactories[pair.first] = pair.second;
    }
}

void ConstructManager::baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, const BuilderManager & builderManager, const ArmyManager & armyManager) {
    // Build targets for each type
    int population = economy.getPopulation();
    int builderTarget = population * .6;
    population -= builderTarget;
    int rangedTarget = population * .2;
    population -= rangedTarget;
    int meleeTarget = population;

    builderTarget -= builderManager.getBuilderCount();
    for (auto pair : builderFactories) {
        if (builderTarget > 0 && economy.charge(BUILDER_UNIT)) {
            actions[pair.first] = Util::getAction(BuildAction(BUILDER_UNIT, Util::getBuildPosition(pair.second)));
            builderTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }

    for (auto pair : rangedFactories) {
        if (rangedTarget > 0 && economy.charge(RANGED_UNIT)) {
            actions[pair.first] = Util::getAction(BuildAction(RANGED_UNIT, Util::getBuildPosition(pair.second)));
            rangedTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }

    for (auto pair : meleeFactories) {
        if (meleeTarget > 0 && economy.charge(MELEE_UNIT)) {
            actions[pair.first] = Util::getAction(BuildAction(MELEE_UNIT, Util::getBuildPosition(pair.second)));
            meleeTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }
}