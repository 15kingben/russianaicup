#ifndef REPAIRABLE_HPP
#define REPAIRABLE_HPP

#include "model/Model.hpp"
#include <vector>
#include <unordered_set>

class Repairable {
    public:
        Repairable();
        virtual Entity getEntity() = 0;
        std::unordered_set<int> helpers;
};

#endif