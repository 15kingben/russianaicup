#include "ArmyManager.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>

ArmyManager::ArmyManager() {
    
}

CombatUnit::CombatUnit() { };

CombatUnit::CombatUnit(Entity entity, CombatStrat strat, Vec2Int target) {
    this->entity = entity;
    this->strat = strat;
    fallback = false;
    this->target = target;
}

Vec2Int ArmyManager::getRandomEnemyTarget() {
    std::random_device mch; 
    std::default_random_engine generator(mch());
    std::uniform_int_distribution<int> distribution(0,3);
    
    Vec2Int me = Util::getHomeDirection();
    me.x = std::max(0, me.x); me.y = std::max(0, me.y);
    // if (done.size() >= Util::playerCount - 1) return Vec2Int(Util::mapSize / 2, Util::mapSize / 2);

    while (true) {
        Vec2Int target;
        int roll = distribution(generator);
        if (roll == 0) target = Vec2Int(0, 0);
        if (roll == 1) target = Vec2Int(1, 0);
        if (roll == 2) target = Vec2Int(0, 1);
        if (roll == 3) target = Vec2Int(1, 1);

        // if (Util::playerCount == 2) {
        //     if (roll == 1 || roll == 2) continue;
        // }

        for (auto pair : done) {
            if (pair == target) continue;
        }

        // TODO: add more logic for if enemies die
        if (!(target == me)) {
            target.x *= Util::mapSize - 1;
            target.y *= Util::mapSize - 1;
            return target;
        } 
    }
}   

int ArmyManager::getMeleeUnitCount() const {
    return melees.size();
}

int ArmyManager::getRangedUnitCount() const {
    return ranged.size();
}

void ArmyManager::checkDone(CombatUnit& unit) {
    int m = Util::mapSize - 1;
    Vec2Int v = unit.entity.position;
    v.x = std::max(v.x, 1);
    v.y = std::max(v.y, 1);
    if (unit.entity.position == Vec2Int(0, 0) || unit.entity.position == Vec2Int(m, 0) || unit.entity.position == Vec2Int(0, m) || unit.entity.position == Vec2Int(m, m)) {
        
        done.emplace(v);
    }

    for (auto pair : done) {
        if (pair == v) {
            unit.target = getRandomEnemyTarget();
            break;
        }
    }
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
            ranged[pair.first] = CombatUnit(pair.second, ATTACK, getRandomEnemyTarget());
        }
        checkDone(ranged[pair.first]);
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
            melees[pair.first] = CombatUnit(pair.second, DEFEND, Vec2Int(0,0));
        }
        checkDone(melees[pair.first]);
    }
}

void ArmyManager::turretActions(std::unordered_map<int, EntityAction> & actions, std::unordered_map<int, Entity> & turrets) {
    int range = Util::entityProperties[TURRET].attack.get()->attackRange;
    for (auto pair : turrets) {
        actions[pair.first] = Util::getAction(Util::getAttackAction(nullptr, 0, std::vector<EntityType>()));
    }
}

void ArmyManager::combatActions(std::unordered_map<int, EntityAction> & actions) {
    for (auto & pair : ranged) {
        if (pair.second.strat == DEFEND) actions[pair.first] = getDefendAction(pair.second);
        if (pair.second.strat == ATTACK) actions[pair.first] = getAttackAction(pair.second);
    }
    for (auto & pair : melees) {
        if (pair.second.strat == DEFEND) actions[pair.first] = getDefendAction(pair.second);
        if (pair.second.strat == ATTACK) actions[pair.first] = getAttackAction(pair.second);
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

    if (unit.fallback) {
        if (Util::dist2(unit.entity.position, Util::homeBase) < Util::dist2(Vec2Int(0,0), Vec2Int(RECOVER_DISTANCE, RECOVER_DISTANCE))) {
            unit.fallback = false;
        } else {
            return action;
        }
    }


    // If close enough to base add attack action
    if (Util::dist2(unit.entity.position, Util::homeBase) > Util::dist2(Vec2Int(0,0), Vec2Int(FALLBACK_DISTANCE, FALLBACK_DISTANCE))) {
        unit.fallback = true;
        return action;
    }

    action.attackAction = std::make_shared<AttackAction>(Util::getAttackAction(nullptr, mapSize/2, defendTargets));

    return action;
}

EntityAction ArmyManager::getAttackAction(CombatUnit & unit) {
    EntityAction action = Util::getAction(MoveAction(unit.target, true, true));
    action.attackAction = std::make_shared<AttackAction>(Util::getAttackAction(nullptr, 10, std::vector<EntityType>()));
    return action;
}