#include "ConstructManager.hpp"
#include "BuilderManager.hpp"
#include "ArmyManager.hpp"
#include "Util.hpp"
#include "Economy.hpp"
#include <iostream>

Square::Square() {
    entity = Entity();
    occupied = false;
}

Square::Square(Entity entity) {
    this->entity = entity;
    occupied = true;
}

void Square::setEntity(Entity entity) {
    this->entity = entity;
    occupied = true;
}

Entity Square::getEntity() {
    return entity;
}

bool Square::isOccupied() {
    return occupied;
}

void Square::setPlanned() {
    occupied = true;
}

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

void ConstructManager::baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, const BuilderManager & builderManager, 
        const ArmyManager & armyManager, std::vector<std::vector<Square> > & open) {
    // Build targets for each type
    int population = economy.getPopulation();
    // all builders if no other factories yet
    int builderTarget = population == 5 ? population : population * .5;
    population -= builderTarget;
    int rangedTarget = population * .5;
    population -= rangedTarget;
    int meleeTarget = population;

    builderTarget -= builderManager.getBuilderCount();
    for (auto pair : builderFactories) {
        if (builderTarget > 0 && economy.charge(BUILDER_UNIT, Util::getUnitCost(BUILDER_UNIT, builderManager.getBuilderCount()))) {
            actions[pair.first] = Util::getAction(BuildAction(BUILDER_UNIT, Util::getBuildPosition(pair.second, open)));
            builderTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }

    rangedTarget -= armyManager.getRangedUnitCount();
    for (auto pair : rangedFactories) {
        if (rangedTarget > 0 && economy.charge(RANGED_UNIT, Util::getUnitCost(RANGED_UNIT, armyManager.getRangedUnitCount()))) {
            actions[pair.first] = Util::getAction(BuildAction(RANGED_UNIT, Util::getBuildPosition(pair.second, open)));
            rangedTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }

    meleeTarget -= armyManager.getMeleeUnitCount();
    for (auto pair : meleeFactories) {
        if (meleeTarget > 0 && economy.charge(MELEE_UNIT, Util::getUnitCost(MELEE_UNIT, armyManager.getMeleeUnitCount()))) {
            actions[pair.first] = Util::getAction(BuildAction(MELEE_UNIT, Util::getBuildPosition(pair.second, open)));
            meleeTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }

    
}