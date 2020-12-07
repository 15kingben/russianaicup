#ifndef COMBAT_UNIT_H
#define COMBAT_UNIT_H

#include "model/Model.hpp"

enum CombatStrat {
    DEFEND,
    ATTACK
};

class CombatUnit {
    public:
        CombatUnit();
        CombatUnit(Entity entity, CombatStrat strat, Vec2Int target);
        CombatStrat strat;
        Vec2Int target;
        Entity entity;
        bool fallback;
};

#endif