#include "ArmyManager.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <vector>

ArmyManager::ArmyManager() {
    
}

CombatUnit::CombatUnit(Entity entity, CombatStrat strat) {
    this->entity = entity;
    this->strat = strat;
}

int ArmyManager::getMeleeUnitCount() const {
    return melees.size();
}

int ArmyManager::getRangedUnitCount() const {
    return ranged.size();
}

void ArmyManager::updateRanged(const std::unordered_map<int, Entity> & currentRanged) {
    // Remove dead ranged from the list
    for (auto it = ranged.begin(); it != ranged.end();) {
        if (currentRanged.find(it->first) == currentRanged.end()) {
            it = ranged.erase(it);
        } else it++;
    }

    // Add new ranged (all miners for now)
    for (auto pair : currentRanged) {
        if (ranged.find(pair.first) != ranged.end()) continue;
        ranged[pair.first] = CombatUnit(pair.second, DEFEND);
    }
}

void ArmyManager::updateMelee(const std::unordered_map<int, Entity> & currentMelee) {
    // Remove dead melees from the list
    for (auto it = melees.begin(); it != melees.end();) {
        if (currentMelee.find(it->first) == currentMelee.end()) {
            it = melees.erase(it);
        } else it++;
    }
 
    // Add new builders (all miners for now)
    for (auto pair : currentMelee) {
        if (melees.find(pair.first) != melees.end()) continue;
        melees[pair.first] = CombatUnit(pair.second, DEFEND);
    }
}

void ArmyManager::turretActions(std::unordered_map<int, EntityAction> & actions, std::unordered_map<int, Entity> & turrets) {
    int range = Util::entityProperties[TURRET].attack.get()->attackRange;
    for (auto pair : turrets) {
        actions[pair.first] = Util::getAction(Util::getAttackAction(nullptr, 0, std::vector<EntityType>()));
    }
}

void ArmyManager::combatActions(std::unordered_map<int, EntityAction> & actions) {
    for (auto pair : ranged) {
        if (pair.second.strat == DEFEND) actions[pair.first] = getDefendAction(pair.second);
    }
}

void ArmyManager::setMaxDistance(int x) {
    MAX_DEFEND_DISTANCE = x;
}

EntityAction ArmyManager::getDefendAction(CombatUnit unit) {
    int mapSize = Util::mapSize;
    EntityAction action = Util::getAction(MoveAction(Vec2Int(mapSize - 20, mapSize - 20), true, false));
    std::vector<EntityType> defendTargets({BUILDER_UNIT, RANGED_UNIT, MELEE_UNIT});

    // If close enough to base add attack action
    if (Util::dist2(unit.entity.position, Vec2Int(mapSize, mapSize)) < Util::dist2(Vec2Int(0,0), Vec2Int(MAX_DEFEND_DISTANCE, MAX_DEFEND_DISTANCE))) {
        action.attackAction = std::make_shared<AttackAction>(Util::getAttackAction(nullptr, mapSize / 3, defendTargets));
    }
}