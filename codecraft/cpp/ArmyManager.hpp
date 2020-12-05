#ifndef _ARMY_MANAGER_HPP_
#define _ARMY_MANAGER_HPP_

#include <unordered_map>
#include "model/Model.hpp"

enum CombatStrat {
    DEFEND,
    ATTACK
};

class CombatUnit {
    public:
        CombatUnit(Entity entity, CombatStrat strat);
        CombatStrat strat;
        Vec2Int target;
        Entity entity;
};

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
    private:
        std::unordered_map<int, CombatUnit> ranged;
        std::unordered_map<int, CombatUnit> melees;
        int MAX_DEFEND_DISTANCE;
        EntityAction getDefendAction(CombatUnit unit);
};

#endif