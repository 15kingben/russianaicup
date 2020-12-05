#include "MyStrategy.hpp"
#include "Economy.hpp"
#include "BuilderManager.hpp"
#include "ConstructManager.hpp"
#include "ArmyManager.hpp"
#include "Util.hpp"
#include <exception>
#include <memory>
#include <string>
#include <sstream> 
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

std::string debugEntityProperty(const EntityProperties &props);
std::string debugEntityType(EntityType e);
std::string debugEntity(const Entity &e);
void takeUpSpace(const Entity& entity, std::vector<std::vector<bool> > &open);
void everyTickInitialization();
void oneTimeInitialization();


// whether or not the square is free
std::vector<std::vector<bool> > open;
// has one time setup been completed
bool oneTimeInitDone = false;
// global pointer to per-tick PlayerView
const PlayerView *pv;
// global object tracking our economy
Economy economy;
// global object to manage builder units
BuilderManager builderManager;
// global object to manage construction
ConstructManager constructManager;
// global object to manage soldiers
ArmyManager armyManager;

unordered_map<int, EntityAction> myAction;
unordered_map<int, Entity> builders;
unordered_map<int, Entity> melees;
unordered_map<int, Entity> ranged;
unordered_map<int, Entity> builderFactories;
unordered_map<int, Entity> rangedFactories;
unordered_map<int, Entity> meleeFactories;
unordered_map<int, Entity> turrets;

MyStrategy::MyStrategy() {}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface)
{
    cout << "pre" << endl;
    pv = &playerView;
    oneTimeInitialization();
    everyTickInitialization();
    cout << "post" << endl;

    int me = playerView.myId;

    for (Entity entity : playerView.entities) {
        takeUpSpace(entity, open);

        if (entity.playerId && *entity.playerId.get() == me) {
            economy.updatePopulation(entity.entityType);

            switch (entity.entityType) {
                case BUILDER_UNIT:
                    builders[entity.id] = entity;
                    break;
                case RANGED_BASE:
                    rangedFactories[entity.id] = entity;
                    break;
                case MELEE_BASE:
                    meleeFactories[entity.id] = entity;
                    break;
                case BUILDER_BASE:
                    builderFactories[entity.id] = entity;
                    break;
                case RANGED_UNIT:
                    ranged[entity.id] = entity;
                    break;
                case MELEE_UNIT:
                    melees[entity.id] = entity;
                    break;
                case TURRET:
                    turrets[entity.id] = entity;
                    break;
            }
        }
    }

    builderManager.updateBuilders(builders);
    constructManager.updateBases(builderFactories, rangedFactories, meleeFactories);
    armyManager.updateMelee(melees);
    armyManager.updateRanged(ranged);

    constructManager.baseBuildActions(myAction, economy, builderManager, armyManager, open);
    builderManager.builderActions(myAction);
    armyManager.turretActions(myAction, turrets);
    armyManager.combatActions(myAction);

    return Action(myAction);
}

void takeUpSpace(const Entity& entity, std::vector<std::vector<bool> > &open) {
    int x = entity.position.x;
    int y = entity.position.y;
    int size = pv->entityProperties.at(entity.entityType).size;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            open[i][j] = false;
        }
    }
}

void everyTickInitialization() {
    int size = pv->mapSize;
    for (auto row : open) {
        for (int i = 0; i < size; i++) {
            row[i] = true;
        }
    }

    myAction.clear();
    builders.clear();
    melees.clear();
    ranged.clear();
    turrets.clear();
    builderFactories.clear();
    rangedFactories.clear();
    meleeFactories.clear();

    economy.setResources(pv->players[pv->myId].resource);
    economy.setPopulation(0);
}

Vec2Int Util::homeBase;

void oneTimeInitialization() {
    if (oneTimeInitDone) return;
    int size = pv->mapSize;
    for (int i = 0; i < size; i++) {
        open.emplace_back(std::vector<bool>());
        for (int j = 0; j < size; j++) {
            open[i].emplace_back(true);
        }
    }

    Util::entityProperties = pv->entityProperties;
    Util::mapSize = pv->mapSize;
    for (auto entity : pv->entities) {
        if (entity.entityType == BUILDER_BASE) Util::homeBase = entity.position;
    }

    armyManager.setMaxDistance(pv->mapSize/3);

    oneTimeInitDone = true;
}










/////////////////////////////////////////////////////
/////////////////// DEBUG ///////////////////////////
/////////////////////////////////////////////////////
// Method only run in local mode, not in competition
void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface)
{
    debugInterface.send(DebugCommand::Clear());

    bool debugEntityProperties = false;
    bool debugEntities = false;

    std::string str;
    
    if (debugEntityProperties) {
        for (auto pair : playerView.entityProperties) {
            str += debugEntityType(pair.first) + ":  ";
            str += debugEntityProperty(pair.second);
            str += "\n";
        }
    }

    if (debugEntities) {
        for (Entity entity : playerView.entities) {
            str += debugEntity(entity);
            str += "\n";
        }
    }

    // debugInterface.send(DebugCommand::Add(std::make_shared<DebugData::Log>(str)));
    std::cout << str << "\n";

    debugInterface.getState();
}



////////////// Debug Utilities  /////////////////////
std::string debugVector(Vec2Int vec) {
    std::string s = "";
    s += "("; s += std::to_string(vec.x); s += ","; s+= std::to_string(vec.y); s += ")";
    return s;
}

std::string debugEntity(const Entity &entity) {
    std::string s = "";
    s += debugEntityType(entity.entityType) + ":  ";
    s += "hp: " + std::to_string(entity.health);  s += ", ";
    s += "id: " + std::to_string(entity.id);  s += ", ";
    s += "pos: " + debugVector(entity.position); s += ", ";
    return s;
}

std::string debugEntityProperty(const EntityProperties &props) {
    std::string s = "";
    s += "cost: ";
    s += std::to_string(props.initialCost);
    s += " | maxhp: ";
    s += std::to_string(props.maxHealth);
    return s;
}

std::string debugEntityType(EntityType type) {
    switch (type) {
        case 0:
            return "wall";
        case 1:
            return "house";
        case 2:
            return "builder_base";
        case 3:
            return "builder_unit";
        case 4:
            return "melee_base";
        case 5:
            return "melee_unit";
        case 6:
            return "ranged_base";
        case 7:
            return "ranged_unit";
        case 8:
            return "resource";
        case 9:
            return "turret";
    }
    return "";
}