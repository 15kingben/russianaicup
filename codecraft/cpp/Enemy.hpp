#ifndef ENEMY_H
#define ENEMY_H

#include "model/Model.hpp"
#include <vector>
#include <deque>
#include "Job.hpp"

class Enemy {
    public:
        Enemy();
        Enemy(int id);
        int id;
        int resources;
        int population;
        Vec2Int homeBase;
        Vec2Int direction;
};
#endif