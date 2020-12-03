#include "ArmyManager.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <vector>

ArmyManager::ArmyManager() {
    
}

int ArmyManager::getMeleeUnitCount() const {
    return melees.size();
}

int ArmyManager::getRangedUnitCount() const {
    return ranged.size();
}

void ArmyManager::updateRanged(const std::unordered_map<int, Entity> & currentRanged) {
    ranged.clear();
    ranged = currentRanged;
}

void ArmyManager::updateMelee(const std::unordered_map<int, Entity> & currentMelee) {
    melees.clear();
    melees = currentMelee;
}

void ArmyManager::turretActions(std::unordered_map<int, EntityAction> & actions, std::unordered_map<int, Entity> & turrets) {
    int range = Util::entityProperties[TURRET].attack.get()->attackRange;
    for (auto pair : turrets) {
        actions[pair.first] = Util::getAction(Util::getAttackAction(nullptr, 0, std::vector<EntityType>()));
    }
}