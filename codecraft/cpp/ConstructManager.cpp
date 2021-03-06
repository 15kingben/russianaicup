#include "ConstructManager.hpp"
#include "BuilderManager.hpp"
#include "ArmyManager.hpp"
#include "Util.hpp"
#include "Economy.hpp"
#include <iostream>
#include <algorithm>
#include "Building.hpp"

Square::Square() {
    entity = Entity();
    entity.id = -1;
    occupied = false;
    danger = false;
    support = 0;
}

Square::Square(Entity entity) {
    this->entity = entity;
    occupied = true;
    danger = false;
    support = 0;
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

Building::Building() { }

Building::Building(Entity entity) {
    this->entity = entity;
}

Entity Building::getEntity() {
    return entity;
}

ConstructManager::ConstructManager() {

}

void fuck(const std::unordered_map<int, Entity> & input, std::unordered_map<int, Building> & output) {
    // Remove dead builders from the list
    for (auto it = output.begin(); it != output.end();) {
        if (input.find(it->first) == input.end()) {
            it = output.erase(it);
        } else it++;
    }

    // Add new builders (all miners for now)
    for (auto pair : input) {
        if (output.find(pair.first) != output.end()) {
            output[pair.first].entity = pair.second;
        } else {
            output[pair.first] = Building(pair.second);
        }
    }
}

void ConstructManager::updateBuildings(const std::unordered_map<int, Entity> & builderBases, 
                    const std::unordered_map<int, Entity> & rangedBases,
                    const std::unordered_map<int, Entity> & meleeBases,
                    const std::unordered_map<int, Entity> & houses,
                    const std::unordered_map<int, Entity> & turrets) {
    fuck(builderBases, builderFactories);
    fuck(rangedBases, rangedFactories);
    fuck(meleeBases, meleeFactories);
    fuck(houses, this->houses);
    fuck(turrets, this->turrets);
}

void ConstructManager::baseBuildActions(std::unordered_map<int, EntityAction> & actions, Economy & economy, BuilderManager & builderManager, 
        const ArmyManager & armyManager, std::vector<std::vector<Square> > & open) {
    // Build targets for each type
    int population = economy.getPopulation();
    // all builders if no other factories yet
    int builderTarget = population <= 15 ? population - 2 : population * .5;
    population -= builderTarget;
    int rangedTarget = population * .7;
    population -= rangedTarget;
    int meleeTarget = population;

    builderTarget -= builderManager.getBuilderCount();
    for (auto pair : builderFactories) {
        if (builderTarget > 0 && economy.charge(BUILDER_UNIT, Util::getUnitCost(BUILDER_UNIT, builderManager.getBuilderCount()))) {
            actions[pair.first] = Util::getAction(BuildAction(BUILDER_UNIT, Util::getBuildPosition(pair.second.entity, open)));
            builderTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }

    rangedTarget -= armyManager.getRangedUnitCount();
    for (auto pair : rangedFactories) {
        if (rangedTarget > 0 && economy.charge(RANGED_UNIT, Util::getUnitCost(RANGED_UNIT, armyManager.getRangedUnitCount()))) {
            actions[pair.first] = Util::getAction(BuildAction(RANGED_UNIT, Util::getBuildPosition(pair.second.entity, open)));
            rangedTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }

    meleeTarget -= armyManager.getMeleeUnitCount();
    for (auto pair : meleeFactories) {
        if (meleeTarget > 0 && economy.charge(MELEE_UNIT, Util::getUnitCost(MELEE_UNIT, armyManager.getMeleeUnitCount()))) {
            actions[pair.first] = Util::getAction(BuildAction(MELEE_UNIT, Util::getBuildPosition(pair.second.entity, open)));
            meleeTarget--;
        } else {
            actions[pair.first] = Util::getEmptyAction();
        }
    }
}

void ConstructManager::initHouseLocations() {
    std::cout << "homeBase: " << Util::homeBase.x  << " " << Util::homeBase.y << std::endl;
    Vec2Int d = Util::getHomeDirection(Util::homeBase); 
    std::cout << "homeBase: " << d.x  << " " << d.y << std::endl;
    Vec2Int startPoint(std::max(d.x, 0) * (Util::mapSize - 1), std::max(d.y, 0) * (Util::mapSize - 1)); std::cout << Util::printVec(startPoint) << std::endl;
    d.x *= -1; d.y *= -1;

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
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
    for (auto & pair : houseLocations) {
        int id = pair.second;
        if (id == -1 || id == 0) continue;
        if (id > 0 && builderManager.builders.find(id) == builderManager.builders.end()) {
            // uh oh builder died before finishing the job
            houseLocations[pair.first] = 0;
        } else if (builderManager.builders.at(id).committed == false) {
            // builder actions happen after contruct actions
            // so we are done with this builder
            pair.second = -1;
        }
        if (id == -1 && open[pair.first.first][pair.first.second].getEntity().id != -1 && open[pair.first.first][pair.first.second].getEntity().entityType != HOUSE) {
            pair.second = 0;
        }
    }


    if (Util::economy->getPopulation() > 160 || (Util::economy->getPopulation() - 2) > Util::economy->getUsedPopulation() || Util::economy->getResources() < 50) {
        // don't build any more houses
        return;
    }

    int inProgress = 0;
    for (auto pair : houseLocations)
        if (pair.second != 0 && pair.second != -1)
            inProgress++;

    if (std::max(inProgress, builderManager.getCommitted()) < builderManager.getBuilderCount() / 5) {
        for (auto & pair : houseLocations) {
            if (pair.second == 0 && Util::getClear(BuildAction(HOUSE, Vec2Int(pair.first.first, pair.first.second)), open).empty()) {
                int x = builderManager.assignNearestWorkerToBuild(Vec2Int(pair.first.first, pair.first.second), HOUSE, open);
                if (x == -1) x = 0;
                pair.second = x;
                break;
            }
        }
    }
}

void ConstructManager::repairBuildings(BuilderManager & builderManager, std::vector<std::vector<Square> > & open) {
    for (auto& pair : houses) {
        builderManager.repair(pair.second, open);
    }
    for (auto& pair : turrets) {
        builderManager.repair(pair.second, open);
    }
    for (auto& pair : builderFactories) {
        builderManager.repair(pair.second, open);
    }
    for (auto& pair : rangedFactories) {
        builderManager.repair(pair.second, open);
    }
    for (auto& pair : meleeFactories) {
        builderManager.repair(pair.second, open);
    }
}