#include "Util.hpp"

EntityAction Util::getAction(BuildAction action) {
    return EntityAction(
            std::shared_ptr<MoveAction>(), 
            std::make_shared<BuildAction>(action), 
            std::shared_ptr<AttackAction>(),
            std::shared_ptr<RepairAction>());
}

EntityAction Util::getAction(AttackAction action) {
    return EntityAction(
            std::shared_ptr<MoveAction>(), 
            std::shared_ptr<BuildAction>(), 
            std::make_shared<AttackAction>(action),
            std::shared_ptr<RepairAction>());
}