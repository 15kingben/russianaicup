#ifndef _ARMY_MANAGER_HPP_
#define _ARMY_MANAGER_HPP_

#include <unordered_map>
#include "CombatUnit.hpp"
#include "model/Model.hpp"
#include <unordered_set>

class ArmyManager {
    public:
        ArmyManager();
        int getRangedUnitCount() const;
        int getMeleeUnitCount() const;
        void updateRanged(const std::unordered_map<int, Entity> & currentRanged);
        void updateMelee(const std::unordered_map<int, Entity> & currentMelee);
        void turretActions(std::unordered_map<int, EntityAction> & actions, std::unordered_map<int, Entity> & turrets);
        void combatActions(std::unordered_map<int, EntityAction> & actions);
        void setMaxDistance(int x);
        Vec2Int getRandomEnemyTarget();
    private:
        std::unordered_map<int, CombatUnit> ranged;
        std::unordered_map<int, CombatUnit> melees;
        int FALLBACK_DISTANCE;
        int RECOVER_DISTANCE;                                                                                                      
        EntityAction getDefendAction(CombatUnit & unit);
        EntityAction getAttackAction(CombatUnit & unit);
        std::unordered_set<Vec2Int> done;
};

#endif