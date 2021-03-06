#include "Util.hpp"
#include "ConstructManager.hpp"
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <string>
#include <random>

std::unordered_map<EntityType, EntityProperties> Util::entityProperties;
Economy* Util::economy;
int Util::myId;
int Util::mapSize;
Vec2Int Util::homeBase;
std::unordered_map<int, Entity> Util::entities;
int Util::playerCount;
int Util::tickCount;

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

std::unordered_set<Vec2Int> Util::getNeighborPositions(Vec2Int bottomLeft, EntityType type) {
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
            return pos;
        }
    }
    return Vec2Int(Util::mapSize / 2, Util::mapSize / 2);;
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
Vec2Int Util::getHomeDirection(Vec2Int hb) {
    int ms = Util::mapSize;
    int x = hb.x < ms / 2 ? -1 : 1;
    int y = hb.y < ms / 2 ? -1 : 1;
    return Vec2Int(x, y);
}

MoveAction Util::getFlee(Entity entity, const std::vector<std::vector<Square> > & map) {
    int dangerZone = 6; // Ranged unit attack range + 1
    // finish
    return MoveAction();
}

std::unordered_set<int> Util::getClear(BuildAction action, std::vector<std::vector<Square> > & map) {
    Vec2Int buildLocation = action.position;
    std::unordered_set<int> ret;
    int sideLength = Util::entityProperties[action.entityType].size;
    for (int x = buildLocation.x; x < buildLocation.x + sideLength; x++) {
        for (int y = buildLocation.y; y < buildLocation.y + sideLength; y++) {
            if (map[x][y].isOccupied()) {  // && (!map[x][y].getEntity().playerId || *map[x][y].getEntity().playerId.get() != Util::myId)
                ret.emplace(map[x][y].getEntity().id);
            }
        }
    }
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

std::string Util::printVec(Vec2Int vec) {
    return std::to_string(vec.x) + " " + std::to_string(vec.y);
}

void Util::printAction(EntityAction action) {
    if (action.attackAction != nullptr) {
        std::cout << "Attack: " << action.attackAction.get()->target << std::endl;
    }
    if (action.moveAction != nullptr) {
        std::cout << "Move: " << printVec(action.moveAction.get()->target) << std::endl;
    }
    if (action.repairAction != nullptr) {
        std::cout << "Repair: " << action.repairAction.get()->target << std::endl;
    }
    if (action.buildAction != nullptr) {
        std::cout << "Build: " << printVec(action.buildAction.get()->position) << std::endl;
    }
}

std::string Util::printEntityType(EntityType type) {
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

std::string Util::printEntity(const Entity &entity) {
    std::string s = "";
    s += printEntityType(entity.entityType) + ":  ";
    s += "hp: " + std::to_string(entity.health);  s += ", ";
    s += "id: " + std::to_string(entity.id);  s += ", ";
    s += "pos: " + printVec(entity.position); s += ", ";
    return s;
}

std::pair<int, int> Util::pair(Vec2Int v) {
    return std::make_pair(v.x, v.y);
}

bool Util::isNeighbor(Vec2Int position, Entity entity) {
    std::unordered_set<Vec2Int> poop = Util::getNeighborPositions(entity.position, entity.entityType);
    bool isNeighbor = false;
    for (auto x : poop) {
        if (x == position) {
            isNeighbor = true;
            break;
        }
    }
    return isNeighbor;
}

void Util::debug(std::string s) {
    if (false) {
        std::cout << s << std::endl;
    }
}

void Util::markDanger(Entity entity, std::vector<std::vector<Square> > & open) {
    Vec2Int pos = entity.position;
    int RANGE = Util::entityProperties[entity.entityType].attack.get()->attackRange + 2;
    for (int x = pos.x - RANGE; x < pos.x + RANGE + 1; x++) {
        for (int y = pos.y - RANGE; y < pos.y + RANGE + 1; y++) {
            if (!inb(x, y)) continue;
            open[x][y].danger = true;
        }
    }
}

void Util::markSupport(Entity entity, std::vector<std::vector<Square> > & open) {
    Vec2Int pos = entity.position;
    int RANGE = 3;
    for (int x = pos.x - RANGE; x < pos.x + RANGE + 1; x++) {
        for (int y = pos.y - RANGE; y < pos.y + RANGE + 1; y++) {
            if (!inb(x, y)) continue;
            open[x][y].support++;
        }
    }
}

Vec2Int Util::getClosestSafeSpace(Vec2Int pos, std::vector<std::vector<Square> > & open) {
    int RANGE = 1;
    for (int x = pos.x - RANGE; x < pos.x + RANGE + 1; x++) {
        for (int y = pos.y - RANGE; y < pos.y + RANGE + 1; y++) {
            if (!inb(x, y) || open[x][y].danger) continue;
            return Vec2Int(x, y);
        }
    }
    return Vec2Int(0, 0);
}

bool Util::getDanger(std::vector<std::vector<Square> > & open) {
    int DIST = 30;
    for (int i = 0; i < DIST; i++) {
        for (int j = 0; j < DIST; j++) {
            if (open[i][j].danger) {
                return true;
            }
        }
    }
    return false;
}