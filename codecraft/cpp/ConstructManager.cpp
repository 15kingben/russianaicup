#include "ConstructManager.hpp"
#include "Util.hpp"

 void ConstructManager::updateBases(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases) {
    meleeFactories = meleeBases;
    rangedFactories = rangedBases;
    builderFactories = builderBases;
}

void ConstructManager::baseBuildActions(std::unordered_map<int, EntityAction> & actions) {
    for (auto pair : builderFactories) {
        actions[pair.first] = Util::getAction(BuildAction(BUILDER_UNIT, Util::getBuildPosition(pair.second)));
    }

    for (auto pair : rangedFactories) {
        actions[pair.first] = Util::getAction(BuildAction(RANGED_UNIT, Util::getBuildPosition(pair.second)));
    }

    for (auto pair : meleeFactories) {
        actions[pair.first] = Util::getAction(BuildAction(MELEE_UNIT, Util::getBuildPosition(pair.second)));
    }
}