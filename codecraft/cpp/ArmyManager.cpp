#include "ArmyManager.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

int Util::mapSize;

ArmyManager::ArmyManager() {
    
}

CombatUnit::CombatUnit() { };

CombatUnit::CombatUnit(Entity entity, CombatStrat strat) {
    this->entity = entity;
    this->strat = strat;
    fallback = false;
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
        if (ranged.find(pair.first) != ranged.end()) {
            ranged[pair.first].entity = pair.second;
        } else {
            ranged[pair.first] = CombatUnit(pair.second, DEFEND);
        }
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
        if (melees.find(pair.first) != melees.end()) {
            melees[pair.first].entity = pair.second;
        } else {
            melees[pair.first] = CombatUnit(pair.second, DEFEND);
        }
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
        if (pair.second.fallback) std::cout << "fffffff" << std::endl;
    }
    for (auto pair : melees) {
        if (pair.second.strat == DEFEND) actions[pair.first] = getDefendAction(pair.second);
    }
}

void ArmyManager::setMaxDistance(int mapSize) {
    FALLBACK_DISTANCE = mapSize / 3;
    RECOVER_DISTANCE = mapSize / 4;
}

EntityAction ArmyManager::getDefendAction(CombatUnit & unit) {
    int mapSize = Util::mapSize;
    EntityAction action = Util::getAction(MoveAction(Util::homeBase, true, false));
    std::vector<EntityType> defendTargets({BUILDER_UNIT, RANGED_UNIT, MELEE_UNIT});

    if (unit.fallback) std::cout << "fallback" << std::endl;

    if (unit.fallback) {
        if (Util::dist2(unit.entity.position, Util::homeBase) < Util::dist2(Vec2Int(0,0), Vec2Int(RECOVER_DISTANCE, RECOVER_DISTANCE))) {
            unit.fallback = false;
            std::cout << "reset"  << RECOVER_DISTANCE << " " << FALLBACK_DISTANCE << std::endl;
        } else {
            return action;
        }
    }


    // If close enough to base add attack action
    if (Util::dist2(unit.entity.position, Util::homeBase) > Util::dist2(Vec2Int(0,0), Vec2Int(FALLBACK_DISTANCE, FALLBACK_DISTANCE))) {
        std::cout << "poop" << std::endl;
        unit.fallback = true;
        return action;
    }

    action.attackAction = std::make_shared<AttackAction>(Util::getAttackAction(nullptr, mapSize/2, defendTargets));

    return action;
}