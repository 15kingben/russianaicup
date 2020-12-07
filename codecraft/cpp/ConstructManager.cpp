#include "ConstructManager.hpp"
#include "BuilderManager.hpp"
#include "ArmyManager.hpp"
#include "Util.hpp"
#include "Economy.hpp"
#include <iostream>
#include <algorithm>

Square::Square() {
    entity = Entity();
    entity.id = -1;
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

void ConstructManager::baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, BuilderManager & builderManager, 
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

void ConstructManager::initHouseLocations() {
    std::cout << "homeBase: " << Util::homeBase.x  << " " << Util::homeBase.y << std::endl;
    Vec2Int d = Util::getHomeDirection(); 
    std::cout << "homeBase: " << d.x  << " " << d.y << std::endl;
    Vec2Int startPoint(std::max(d.x, 0) * (Util::mapSize - 1), std::max(d.y, 0) * (Util::mapSize - 1)); std::cout << Util::printVec(startPoint) << std::endl;
    d.x *= -1; d.y *= -1;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // filter out base locations
            if ((i==1 || i==2) && (j == 1 || j == 2 || j == 4)) continue;
            if ((j==1 || j==2) && (i == 1 || i == 2 || i == 4)) continue;

            Vec2Int loc(startPoint.x + i * d.x * 4, startPoint.y + j * d.y * 4);
            houseLocations[std::make_pair(loc.x, loc.y)] = 0;
        }
    }

    for (int i = 4; i < 7; i++) {
        for (int j = 4; j < 7; j++) {
            // filter out base locations
            if ((i==1 || i==2) && (j == 1 || j == 2 || j == 4)) continue;
            if ((j==1 || j==2) && (i == 1 || i == 2 || i == 4)) continue;

            Vec2Int loc(startPoint.x + i * d.x * 4, startPoint.y + j * d.y * 4);
            houseLocations[std::make_pair(loc.x, loc.y)] = 0;
        }
    }

    std::cout << "Debug house locations" << std::endl;
    for (auto pair : houseLocations) { 
        std::cout << pair.first.first << " " << pair.first.second << std::endl;
    }
}

void ConstructManager::updateHouseBuilds(BuilderManager& builderManager, std::vector<std::vector<Square> > & open) {
    for (auto pair : houseLocations) {
        int id = pair.second;
        if (builderManager.builders.find(id) == builderManager.builders.end()) {
            // uh oh builder died before finishing the job
            houseLocations[pair.first] = 0;
        } else if (builderManager.builders.at(id).committed == false) {
            // builder actions happen after contruct actions
            // so we are done with this builder
            houseLocations[pair.first] = -1;
        }
    }


    std::cout << "popop" << Util::economy->getPopulation() << " " << Util::economy->getUsedPopulation() << std::endl;
    if (Util::economy->getPopulation() > 110 || Util::economy->getPopulation() > Util::economy->getUsedPopulation()) {
        // don't build any more houses
        return;
    }

    int inProgress = 0;
    for (auto pair : houseLocations)
        if (pair.second != 0 && pair.second != -1)
            inProgress++;

    if (inProgress < builderManager.getBuilderCount() / 5) {
        for (auto & pair : houseLocations) {
            if (pair.second == 0) {
                std::cout << "Building new house" << pair.first.first << " " << pair.first.second << std::endl;
                pair.second = builderManager.assignNearestWorkerToBuild(Vec2Int(pair.first.first, pair.first.second), HOUSE, open);
                break;
            }
        }
    }
}