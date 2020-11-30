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

    std::string str;
    for (auto pair : playerView.entityProperties) {
        str += debugEntityType(pair.first) + ":  ";
        str += debugEntityProperty(pair.second);
        str += "\n";
    }

    // debugInterface.send(DebugCommand::Add(std::make_shared<DebugData::Log>(str)));
    std::cout << str << "\n";

    debugInterface.getState();
}

/////////////////////////////////////////////////////
////////////// debug utilities  /////////////////////
/////////////////////////////////////////////////////

std::string debugEntityProperty(const EntityProperties &props) {
    std::string s = "";
    s += "cost: " + std::to_string(props.cost);
    s += "  maxhp: " + std::to_string(props.maxHealth);
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