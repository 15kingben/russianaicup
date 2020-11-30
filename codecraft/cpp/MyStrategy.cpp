#include "MyStrategy.hpp"
#include <exception>
#include <memory>
#include <string>
#include <sstream> 
#include <iostream>

MyStrategy::MyStrategy() {}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface)
{
    return Action(std::unordered_map<int, EntityAction>());
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface)
{
    debugInterface.send(DebugCommand::Clear());

    bool debugEntityProperties = false;
    bool debugEntities = true;

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

/////////////////////////////////////////////////////
////////////// debug utilities  /////////////////////
/////////////////////////////////////////////////////

std::string debugVector(Vec2Int vec) {
    std::string s = "";
    s += "("; s += vec.x; s += ","; s+= vec.y; s += ")";
    return s;
}

std::string debugEntity(const Entity &entity) {
    std::string s = "";
    s += debugEntityType(entity.entityType) + ":  ";
    s += "hp: " + entity.health;  s += ", ";
    s += "id: " + entity.id;  s += ", ";
    s += "pos: " + debugVector(entity.position); s += ", ";
    return s;
}

std::string debugEntityProperty(const EntityProperties &props) {
    std::string s = "";
    s += "cost: ";
    s += std::to_string(props.cost);
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