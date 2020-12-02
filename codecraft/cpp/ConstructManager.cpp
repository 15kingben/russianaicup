#include "ConstructManager.hpp"

 void ConstructManager::updateBases(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases) {
    meleeFactories = meleeBases;
    rangedFactories = rangedBases;
    builderFactories = builderBases;
}

void ConstructManager::baseBuildActions(std::unordered_map<int, EntityAction> & actions) {
    for (auto pair : meleeFactories) {
        
    }
}