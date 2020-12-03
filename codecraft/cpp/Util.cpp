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
Vec2Int Util::getBuildPosition(Entity entity, std::vector<std::vector<bool> > & open) {
    int size = entityProperties[entity.entityType].size;
    for (int x = entity.position.x; x < entity.position.x + size; x++) {
        int y = entity.position.y - 1;
        if (open[x][y]) {
            open[x][y] = false;
            return Vec2Int(x, y);
        }
        y = entity.position.y + size;
        if (open[x][y]) {
            open[x][y] = false;
            return Vec2Int(x, y);
        }
    }

    for (int y = entity.position.y; y < entity.position.y + size; y++) {
        int x = entity.position.x - 1;
        if (open[x][y]) {
            open[x][y] = false;
            return Vec2Int(x, y);
        }
        x = entity.position.x + size;
        if (open[x][y]) {
            open[x][y] = false;
            return Vec2Int(x, y);
        }
    }
    return Vec2Int(entity.position.x + size, entity.position.y + size - 1);
}