#ifndef JOB_H
#define JOB_H

#include "model/Model.hpp"
#include <vector>
#include <deque>
#include "Square.hpp"

class Job {
    public:
        Job();
        std::deque<EntityAction> actions;
        EntityAction getAction(std::vector<std::vector<Square> > & open);
        Entity entity;
};

#endif