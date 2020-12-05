#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <unordered_map>
#include "model/Model.hpp"
#include <vector>

class Util {
public:
    static std::unordered_map<EntityType, EntityProperties> entityProperties;
    static int mapSize;
    static EntityAction getAction(BuildAction action);
    static EntityAction getAction(AttackAction action);
    static EntityAction getAction(MoveAction action);
    static EntityAction getEmptyAction();
    static Vec2Int getBuildPosition(Entity entity, std::vector<std::vector<bool> > & open);
    static AttackAction getAttackAction(std::shared_ptr<int> target, int pathfindRange, std::vector<EntityType> validTargets);
    static int dist2(Vec2Int from, Vec2Int to);
    static int getUnitCost(EntityType type, int number);
};

#endif