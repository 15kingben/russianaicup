#ifndef _BUILDER_MANAGER_HPP_
#define _BUILDER_MANAGER_HPP_

#include <unordered_map>
#include "model/Model.hpp"
#include <deque>

enum Role {
    MINE = 0,
    BUILD = 1
};

class Builder {
    public:
        Role role;
        bool committed;
        Entity entity;
        Builder();
        Builder(Entity entity, Role role);
};

class BuildJob {
    public:
        std::deque<EntityAction> actions;
};

class BuilderManager {
    public:
        BuilderManager();
        int getBuilderCount() const;
        void updateBuilders(const std::unordered_map<int, Entity> & currentBuilders);
        void builderActions(std::unordered_map<int, EntityAction> & actions);
    private:
        std::unordered_map<int, Builder> builders;
};

#endif