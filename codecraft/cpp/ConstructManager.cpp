#include "ConstructManager.hpp"
#include "Util.hpp"
#include "Economy.hpp"

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

void ConstructManager::baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy economy) {
    for (auto pair : builderFactories) {
        if (economy.charge(BUILDER_UNIT))
            actions[pair.first] = Util::getAction(BuildAction(BUILDER_UNIT, Util::getBuildPosition(pair.second)));
        else
            actions[pair.first] = Util::getEmptyAction();
    }

    for (auto pair : rangedFactories) {
        if (economy.charge(RANGED_UNIT))
            actions[pair.first] = Util::getAction(BuildAction(RANGED_UNIT, Util::getBuildPosition(pair.second)));
        else
            actions[pair.first] = Util::getEmptyAction();
    }

    for (auto pair : meleeFactories) {
        if (economy.charge(MELEE_UNIT))
            actions[pair.first] = Util::getAction(BuildAction(MELEE_UNIT, Util::getBuildPosition(pair.second)));
        else
            actions[pair.first] = Util::getEmptyAction();
    }
}