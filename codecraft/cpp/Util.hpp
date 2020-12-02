#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <unordered_map>
#include "model/Model.hpp"

class Util {
public:
    static EntityAction getAction(BuildAction action);
    static EntityAction getAction(AttackAction action);
};

#endif