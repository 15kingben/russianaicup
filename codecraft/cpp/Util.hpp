#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <unordered_map>
#include "model/Model.hpp"

class Util {
public:
    static std::unordered_map<EntityType, EntityProperties> entityProperties;
    static EntityAction getAction(BuildAction action);
    static EntityAction getAction(AttackAction action);
    static Vec2Int getBuildPosition(Entity entity);
};

#endif