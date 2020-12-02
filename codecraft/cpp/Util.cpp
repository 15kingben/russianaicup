#include "Util.hpp"

std::unordered_map<EntityType, EntityProperties> Util::entityProperties;

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

EntityAction Util::getEmptyAction() {
    return EntityAction(
            std::shared_ptr<MoveAction>(), 
            std::shared_ptr<BuildAction>(), 
            std::shared_ptr<AttackAction>(),
            std::shared_ptr<RepairAction>());
}

// gets the position where new units/buildings can be built from this unit
Vec2Int Util::getBuildPosition(Entity entity) {
    int size = entityProperties[entity.entityType].size;
    return Vec2Int(entity.position.x + size, entity.position.y);
}