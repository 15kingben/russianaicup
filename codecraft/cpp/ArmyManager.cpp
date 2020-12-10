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

Enemy::Enemy() {

}

Enemy::Enemy(int id) {
    this->id = id;
    this->population = 0;
    this->resources = 0;
    this->homeBase = Vec2Int(-1,-1);
} 

int ArmyManager::getMeleeUnitCount() const {
    return melees.size();
}

int ArmyManager::getRangedUnitCount() const {
    return ranged.size();
}

void ArmyManager::updateTarget(CombatUnit& unit) {
    if (target == -1 || enemies[target].homeBase.x == -1) {
        unit.target = Vec2Int(Util::mapSize/2, Util::mapSize / 2);
    } else {
        unit.target = enemies[target].homeBase;
    }
}

CombatUnit ArmyManager::createNewCombatUnit(Entity entity) {
    int population = std::max(0, Util::economy->getPopulation() - 20);
    int targetDefense = 20;
    int targetOffense = population;
    if (defenderCount() < targetDefense) {
        return CombatUnit(entity, DEFEND, Vec2Int(0,0));
    } else {
        return CombatUnit(entity, ATTACK, Vec2Int(0,0));
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
            ranged[pair.first] = createNewCombatUnit(pair.second);
        }
        updateTarget(ranged[pair.first]);
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
            melees[pair.first] = createNewCombatUnit(pair.second);
        }
        updateTarget(melees[pair.first]);
    }
}

void ArmyManager::turretActions(std::unordered_map<int, EntityAction> & actions, std::unordered_map<int, Entity> & turrets) {
    Util::debug("turret actions");
    for (auto pair : turrets) {
        actions[pair.first] = Util::getAction(Util::getAttackAction(nullptr, 0, std::vector<EntityType>()));
    }
}

void ArmyManager::combatActions(std::unordered_map<int, EntityAction> & actions, std::vector<std::vector<Square> > & open) {
    Util::debug("combat actions");
    for (auto & pair : ranged) {
        if (pair.second.strat == DEFEND) actions[pair.first] = getDefendAction(pair.second, open);
        if (pair.second.strat == ATTACK) actions[pair.first] = getAttackAction(pair.second, open);
    }
    for (auto & pair : melees) {
        if (pair.second.strat == DEFEND) actions[pair.first] = getDefendAction(pair.second, open);
        if (pair.second.strat == ATTACK) actions[pair.first] = getAttackAction(pair.second, open);
    }
}

void ArmyManager::setMaxDistance(int mapSize) {
    FALLBACK_DISTANCE = mapSize / 3;
    RECOVER_DISTANCE = mapSize / 4;
}

EntityAction ArmyManager::getDefendAction(CombatUnit & unit, std::vector<std::vector<Square> > & open) {
    int mapSize = Util::mapSize;
    EntityAction action = Util::getAction(MoveAction(Vec2Int(22,22), true, false));
    std::vector<EntityType> defendTargets({BUILDER_UNIT, RANGED_UNIT, MELEE_UNIT});

    Vec2Int p = unit.entity.position;

    if (unit.fallback) {
        if (Util::dist2(unit.entity.position, Util::homeBase) < Util::dist2(Vec2Int(0,0), Vec2Int(RECOVER_DISTANCE, RECOVER_DISTANCE))) {
            unit.fallback = false;
        } else if (open[p.x][p.y].danger && open[p.x][p.y].support < 2){
            // hold ground
            return Util::getAction(Util::getAttackAction(nullptr, 0, defendTargets));
        } else {   
            return action;
        }
    }

    // If close enough to base add attack action
    if (Util::dist2(unit.entity.position, Util::homeBase) > Util::dist2(Vec2Int(0,0), Vec2Int(FALLBACK_DISTANCE, FALLBACK_DISTANCE))) {
        unit.fallback = true;
        return action;
    }

    if (!pacifist) action.attackAction = std::make_shared<AttackAction>(Util::getAttackAction(nullptr, mapSize/2, defendTargets));

    return action;
}

EntityAction ArmyManager::getAttackAction(CombatUnit & unit, std::vector<std::vector<Square> > & open) {
    Util::debug(std::to_string(unit.strat));
    Util::debug(Util::printVec(unit.target));
    EntityAction action = Util::getAction(MoveAction(unit.target, true, true));
    if (!pacifist) action.attackAction = std::make_shared<AttackAction>(Util::getAttackAction(nullptr, 10, std::vector<EntityType>()));

    return action;
}

int getWeakestEnemy(std::unordered_map<int, Enemy> enemies) {
    int maxPop = 10000; int best = -1;
    for (auto pair : enemies) {
        if (pair.second.homeBase.x == -1) continue;
        if (pair.second.population < maxPop) {
            maxPop = pair.second.population;
            best = pair.first;
        }
    }
    return best;
}

void ArmyManager::updateEnemyStatus(const std::vector<std::vector<Square> > & open) {
    
    std::unordered_map<int, Enemy> currentEnemies;
    for (auto & e : Util::entities) {
        if (!e.second.playerId || *e.second.playerId == Util::myId) continue;
        int eid = *e.second.playerId;
        if (currentEnemies.find(eid) == currentEnemies.end()) {
            currentEnemies[eid] = Enemy(eid);
        }

        int pop = Util::entityProperties[e.second.entityType].populationProvide;
        currentEnemies[eid].population += pop;

        if (e.second.entityType == BUILDER_BASE) {
            currentEnemies[eid].homeBase = e.second.position;
            currentEnemies[eid].direction = Util::getHomeDirection(e.second.position);
        }
    }

    this->enemies = currentEnemies;
    // Update new target
    
    if (target == -1) {
        target = getWeakestEnemy(enemies);
    } else {
        if (attackerCount() < 15 || enemies.find(target) == enemies.end() || enemies.find(target)->second.homeBase.x == -1) {
            target = getWeakestEnemy(enemies);
        }
    }
}

int ArmyManager::defenderCount() {
    int s = 0;
    for (auto & x : ranged) {
        if (x.second.strat == DEFEND) s++;
    }
    return s;
}

int ArmyManager::attackerCount() {
    int s = 0;
    for (auto & x : ranged) {
        if (x.second.strat == ATTACK) s++;
    }
    return s + melees.size();
}

int ArmyManager::getTarget() {
    return target;
}