#define DEBUG
#ifdef DEBUG
#include "StringStream.hpp"
#endif

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


    #ifdef DEBUG
    StringStream stream;

    for (auto pair : playerView.entityProperties) {
        stream.write(pair.first);
        stream.write(": ");
        pair.second.writeTo(stream);
        stream.write("\n");
    }

    std::shared_ptr<DebugData> debugData = std::make_shared<DebugData::Log>(stream.get());
    std::cout << stream.get() << std::endl;
    stream.flush();
    debugInterface.send(DebugCommand::Add(debugData));
    #endif //DEBUG


    debugInterface.getState();
}