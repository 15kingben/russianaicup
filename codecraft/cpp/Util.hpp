#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <unordered_map>
#include "model/Model.hpp"
#include <vector>
#include "ConstructManager.hpp"
#include <string>
#include <unordered_set>

class Util {
public:
    static std::unordered_map<EntityType, EntityProperties> entityProperties;
    static int mapSize;
    static int myId;
    static std::unordered_map<int, Entity> entities;
    static Vec2Int homeBase;
    static Economy * economy;
    static int playerCount;
    static int tickCount;
    static EntityAction getAction(BuildAction action);
    static EntityAction getAction(AttackAction action);
    static EntityAction getAction(MoveAction action);
    static EntityAction getAction(RepairAction action);
    static EntityAction getEmptyAction();
    static Vec2Int getBuildPosition(Entity entity, std::vector<std::vector<Square> > & open);
    static AttackAction getAttackAction(std::shared_ptr<int> target, int pathfindRange, std::vector<EntityType> validTargets);
    static int dist2(Vec2Int from, Vec2Int to);
    static int getUnitCost(EntityType type, int number);
    static Vec2Int getHomeDirection(Vec2Int homebase);
    static MoveAction getFlee(Entity entity, const std::vector<std::vector<Square> > & map);
    static std::unordered_set<int> getClear(BuildAction action, std::vector<std::vector<Square> > & map);
    static bool isAdjacent(Vec2Int a, Vec2Int b);
    static Vec2Int getBuildPosition(Vec2Int pos, EntityType type, std::vector<std::vector<Square> > & open);
    static std::string printVec(Vec2Int vec);
    static void printAction(EntityAction action);
    static std::string printEntityType(EntityType type);
    static std::string printEntity(const Entity &entity);
    static std::unordered_set<Vec2Int> getNeighborPositions(Vec2Int bottomLeft, EntityType type);
    static std::pair<int, int> pair(Vec2Int);
    static bool isNeighbor(Vec2Int pos, Entity target);
    static void debug(std::string s);
    static void markDanger(Entity entity, std::vector<std::vector<Square> > & open);
    static void markSupport(Entity entity, std::vector<std::vector<Square> > & open);
    static Vec2Int getClosestSafeSpace(Vec2Int v, std::vector<std::vector<Square> > & open);
    static bool getDanger(std::vector<std::vector<Square> > & open);
};

#endif