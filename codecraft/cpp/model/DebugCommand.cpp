#include "DebugCommand.hpp"
#include <stdexcept>

DebugCommand::Add::Add() { }
DebugCommand::Add::Add(std::shared_ptr<DebugData> data) : data(data) { }
DebugCommand::Add DebugCommand::Add::readFrom(InputStream& stream) {
    DebugCommand::Add result;
    result.data = DebugData::readFrom(stream);
    return result;
}
void DebugCommand::Add::writeTo(OutputStream& stream) const {
    stream.write(TAG);
    data->writeTo(stream);
}

DebugCommand::Clear::Clear() { }
DebugCommand::Clear DebugCommand::Clear::readFrom(InputStream& stream) {
    DebugCommand::Clear result;
    return result;
}
void DebugCommand::Clear::writeTo(OutputStream& stream) const {
    stream.write(TAG);
}
std::shared_ptr<DebugCommand> DebugCommand::readFrom(InputStream& stream) {
    switch (stream.readInt()) {
    case 0:
        return std::shared_ptr<DebugCommand::Add>(new DebugCommand::Add(DebugCommand::Add::readFrom(stream)));
    case 1:
        return std::shared_ptr<DebugCommand::Clear>(new DebugCommand::Clear(DebugCommand::Clear::readFrom(stream)));
    default:
        throw std::runtime_error("Unexpected tag value");
    }
};
