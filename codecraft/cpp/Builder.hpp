#ifndef BUILDER_H
#define BUILDER_H

#include "model/Model.hpp"
#include <vector>
#include <deque>
#include "Job.hpp"

enum Role {
    MINE = 0,
    POP_GROWTH
};

class Builder {
    public:
        Role role;
        bool committed;
        Entity entity;
        Builder();
        Builder(Entity entity, Role role);
        Job job;
};
#endif