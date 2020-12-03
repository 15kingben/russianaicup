#include "BuilderManager.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

BuilderManager::BuilderManager() {
    
}

Builder::Builder(Entity entity, Role role) {
    this->entity = entity;
    this->role = role;
    committed = false;
}

Builder::Builder() {

}

int BuilderManager::getBuilderCount() const {
    return builders.size();
}

void BuilderManager::updateBuilders(const std::unordered_map<int, Entity> & currentBuilders) {
    // Remove dead builders from the list
    for (auto it = builders.begin(); it != builders.end();) {
        if (currentBuilders.find(it->first) == currentBuilders.end()) {
            it = builders.erase(it);
        } else it++;
    }

    // Add new builders (all miners for now)
    for (auto pair : currentBuilders) {
        if (builders.find(pair.first) != builders.end()) continue;
        builders[pair.first] = Builder(pair.second, MINE);
    }
}

void BuilderManager::builderActions(std::unordered_map<int, EntityAction> & actions) {
    for (auto pair : builders) {
        actions[pair.first] = Util::getAction(AttackAction(nullptr, std::make_shared<AutoAttack>(AutoAttack(1000, std::vector<EntityType>()))));
    }
}
