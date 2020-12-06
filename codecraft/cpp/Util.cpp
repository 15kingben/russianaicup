#include "Util.hpp"
#include "ConstructManager.hpp"
#include <algorithm>
#include <unordered_set>

std::unordered_map<EntityType, EntityProperties> Util::entityProperties;
Economy Util::economy;
int Util::myId;
int Util::mapSize;
Vec2Int Util::homeBase;

EntityAction Util::getAction(BuildAction action) {
    return EntityAction(
            nullptr, 
            std::make_shared<BuildAction>(action), 
            nullptr,
            nullptr);
}

EntityAction Util::getAction(AttackAction action) {
    return EntityAction(
            nullptr, 
            nullptr, 
            std::make_shared<AttackAction>(action),
            nullptr);
}

EntityAction Util::getAction(MoveAction action) {
    return EntityAction(
            std::make_shared<MoveAction>(action), 
            nullptr, 
            nullptr,
            nullptr);
}

EntityAction Util::getAction(RepairAction action) {
    return EntityAction(
            nullptr, 
            nullptr, 
            nullptr,
            std::make_shared<RepairAction>(action));
}

EntityAction Util::getEmptyAction() {
    return EntityAction(
            nullptr, 
            nullptr, 
            nullptr,
            nullptr);
}

AttackAction Util::getAttackAction(std::shared_ptr<int> target, int pathfindRange, std::vector<EntityType> validTargets) {
    return AttackAction(target, std::make_shared<AutoAttack>(pathfindRange, validTargets));
}

bool inb(int x, int y) {
    return x >= 0 && y >= 0 && y < Util::mapSize && x < Util::mapSize;
}

std::unordered_set<Vec2Int> getNeighborPositions(Vec2Int bottomLeft, EntityType type) {
    std::unordered_set<Vec2Int> positions;
    int size = Util::entityProperties[type].size;

    for (int x = bottomLeft.x; x < bottomLeft.x + size; x++) {
        int y = bottomLeft.y - 1;
        if (inb(x,y)) {
            positions.emplace(Vec2Int(x,y));
        }
        y = bottomLeft.y + size;
        if (inb(x,y)) {
            positions.emplace(Vec2Int(x,y));
        }
    }

    for (int y = bottomLeft.y; y < bottomLeft.y + size; y++) {
        int x = bottomLeft.x - 1;
        if (inb(x,y)) {
            positions.emplace(Vec2Int(x,y));
        }
        x = bottomLeft.x + size;
        if (inb(x,y)) {
            positions.emplace(Vec2Int(x,y));
        }
    }

    return positions;
}

// gets the position where new units/buildings can be built from this unit
Vec2Int Util::getBuildPosition(Entity entity, std::vector<std::vector<Square> > & open) {
    int size = entityProperties[entity.entityType].size;
    for (Vec2Int pos : getNeighborPositions(entity.position, entity.entityType)) {
        if (!open[pos.x][pos.y].isOccupied()) {
            open[pos.x][pos.y].setPlanned();
            return pos;
        }
    }
    return Vec2Int(entity.position.x + size, entity.position.y + size - 1);
}

int Util::dist2(Vec2Int from, Vec2Int to) {
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    return dy*dy + dx*dx;
}

int Util::getUnitCost(EntityType type, int count) {
    return entityProperties[type].initialCost + count;
}

// Direction of home base, [-1,-1], [-1,1], [1,-1] or [1,1]
Vec2Int Util::getHomeDirection() {
    Vec2Int hb = Util::homeBase;
    int ms = Util::mapSize;
    int x = hb.x < ms / 2 ? 1 : -1;
    int y = hb.y < ms / 2 ? 1 : -1;
    return Vec2Int(x, y);
}

MoveAction Util::getFlee(Entity entity, const std::vector<std::vector<Square> > & map) {
    int dangerZone = 6; // Ranged unit attack range + 1
    // finish
    return MoveAction();
}

Entity Util::getClear(BuildAction action, std::vector<std::vector<Square> > & map) {
    Vec2Int buildLocation = action.position;
    int sideLength = Util::entityProperties[action.entityType].size;
    for (int x = buildLocation.x; x < buildLocation.x + sideLength; x++) {
        for (int y = buildLocation.y; y < buildLocation.y + sideLength; y++) {
            if (map[x][y].isOccupied()) {
                return map[x][y].getEntity();
            }
        }
    }
    Entity ret;
    ret.id = -1;
    return ret;
}

bool Util::isAdjacent(Vec2Int a, Vec2Int b) {
    return abs(a.x - b.x) + abs(a.y - b.y) == 1;
}

Vec2Int Util::getBuildPosition(Vec2Int pos, EntityType type, std::vector<std::vector<Square> > & open) {
    int size = Util::entityProperties[type].size;
    for (Vec2Int pos : getNeighborPositions(pos, type)) {
        if (!open[pos.x][pos.y].isOccupied()) {
            return pos;
        }
    }

    return Vec2Int(-1,-1);
}
