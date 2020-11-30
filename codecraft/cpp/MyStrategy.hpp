#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "DebugInterface.hpp"
#include "model/Model.hpp"

class MyStrategy {
public:
    MyStrategy();
    Action getAction(const PlayerView& playerView, DebugInterface* debugInterface);
    void debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface);
};

std::string debugEntityProperty(const EntityProperties &props);
std::string debugEntityType(EntityType e);
std::string debugEntity(const Entity &e);

#endif