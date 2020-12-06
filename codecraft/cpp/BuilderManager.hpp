#ifndef _BUILDER_MANAGER_HPP_
#define _BUILDER_MANAGER_HPP_

#include <unordered_map>
#include "model/Model.hpp"
#include <deque>
#include "ConstructManager.hpp"

enum Role {
    MINE = 0,
    POP_GROWTH
};

class Job {
    public:
        std::deque<EntityAction> actions;
        EntityAction getAction(std::vector<std::vector<Square> > & open);
        Entity entity;
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

class BuilderManager {
    public:
        BuilderManager();
        int getBuilderCount() const;
        void updateBuilders(const std::unordered_map<int, Entity> & currentBuilders);
        void builderActions(std::unordered_map<int, EntityAction> & actions, std::vector<std::vector<Square> > & open);
        std::unordered_map<int, Builder> builders;
        int assignNearestWorkerToBuild(Vec2Int location, EntityType type, std::vector<std::vector<Square> > & open);
};

#endif