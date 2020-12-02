#include "BuilderManager.hpp"
#include <memory>
#include <string>
#include <vector>

BuilderManager::BuilderManager() {
    
}

Builder::Builder(Entity entity, Role role) {
    this->entity = entity;
    this->role = role;
    committed = false;
}

int BuilderManager::getBuilderCount() {
    return builders.size();
}

void BuilderManager::updateBuilders(const std::unordered_map<int, Entity> & currentBuilders) {
    // Remove dead builders from the list
    for (auto pair : builders) {
        if (currentBuilders.find(pair.first) == currentBuilders.end()) {
            builders.erase(pair.first);
        }
    }

    // Add new builders (all miners for now)
    for (auto pair : currentBuilders) {
        if (builders.find(pair.first) != builders.end()) continue;
        Builder builder(pair.second, MINE);
        builder.entity = pair.second;
        builders[pair.first] = builder;
    }
}

void BuilderManager::builderActions(std::unordered_map<int, EntityAction> & actions) {
    for (auto pair : builders) {
        actions[pair.first] = EntityAction(
            std::shared_ptr<MoveAction>(), 
            std::shared_ptr<BuildAction>(), 
            std::make_shared<AttackAction>(std::shared_ptr<int>(), std::make_shared<AutoAttack>(AutoAttack(10, std::vector<EntityType>()))), 
            std::shared_ptr<RepairAction>());
    }
}
