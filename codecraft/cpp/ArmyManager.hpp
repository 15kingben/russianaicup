#ifndef _ARMY_MANAGER_HPP_
#define _ARMY_MANAGER_HPP_

#include <unordered_map>
#include "CombatUnit.hpp"
#include "model/Model.hpp"
#include <unordered_set>
#include "Square.hpp"
#include "Enemy.hpp"

class ArmyManager {
    public:
        ArmyManager();
        int getRangedUnitCount() const;
        int getMeleeUnitCount() const;
        void updateRanged(const std::unordered_map<int, Entity> & currentRanged);
        void updateMelee(const std::unordered_map<int, Entity> & currentMelee);
        void turretActions(std::unordered_map<int, EntityAction> & actions, std::unordered_map<int, Entity> & turrets);
        void combatActions(std::unordered_map<int, EntityAction> & actions, std::vector<std::vector<Square> > & open);
        void setMaxDistance(int x);
        Vec2Int getRandomEnemyTarget();
        void updateEnemyStatus(const std::vector<std::vector<Square> > & open);
        int getTarget();
    private:
        std::unordered_map<int, CombatUnit> ranged;
        std::unordered_map<int, CombatUnit> melees;
        int FALLBACK_DISTANCE;
        int RECOVER_DISTANCE;                                                                                                      
        EntityAction getDefendAction(CombatUnit & unit, std::vector<std::vector<Square> > & open);
        EntityAction getAttackAction(CombatUnit & unit, std::vector<std::vector<Square> > & open);
        std::unordered_map<int, Enemy> enemies;
        void updateTarget(CombatUnit& unit);
        int defenderCount();
        int attackerCount();
        int target = -1;
        CombatUnit createNewCombatUnit(Entity entity);
        bool pacifist = false;
};

#endif