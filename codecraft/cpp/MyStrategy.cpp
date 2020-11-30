#include "MyStrategy.hpp"
#include <exception>
#include <memory>

MyStrategy::MyStrategy() {}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface)
{
    return Action(std::unordered_map<int, EntityAction>());
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface)
{
    debugInterface.send(DebugCommand::Clear());
    std::shared_ptr<DebugData> debugData = std::make_shared<DebugData::Log>("testaroni");
    debugInterface.send(DebugCommand::Add(debugData));
    debugInterface.getState();
}