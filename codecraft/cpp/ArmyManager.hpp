#ifndef _ARMY_MANAGER_HPP_
#define _ARMY_MANAGER_HPP_

#include <unordered_map>
#include "model/Model.hpp"

class ArmyManager {
    public:
        ArmyManager();
        int getRangedUnitCount() const;
        int getMeleeUnitCount() const;
        void updateRanged(const std::unordered_map<int, Entity> & currentRanged);
        void updateMelee(const std::unordered_map<int, Entity> & currentMelee);
    private:
        std::unordered_map<int, Entity> ranged;
        std::unordered_map<int, Entity> melees;
};

#endif