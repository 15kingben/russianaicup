#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <unordered_map>
#include "model/Model.hpp"
#include <vector>

class Util {
public:
    static std::unordered_map<EntityType, EntityProperties> entityProperties;
    static EntityAction getAction(BuildAction action);
    static EntityAction getAction(AttackAction action);
    static EntityAction getEmptyAction();
    static Vec2Int getBuildPosition(Entity entity, std::vector<std::vector<bool> > & open);
    static AttackAction getAttackAction(std::shared_ptr<int> target, int pathfindRange, std::vector<EntityType> validTargets);
};

#endif