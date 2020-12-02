#ifndef _BUILDER_MANAGER_HPP_
#define _BUILDER_MANAGER_HPP_

#include <unordered_map>
#include "model/Model.hpp"

enum Role {
    MINE = 0,
    BUILD = 1
};

class Builder {
    public:
        Role role;
        bool committed;
        Entity entity;

        Builder(Entity entity, Role role);
};

class BuilderManager {
    public:
        BuilderManager();
        int getBuilderCount();
        void updateBuilders(std::unordered_map<int, Entity> currentBuilders);
        void builderActions(std::unordered_map<int, EntityAction> actions);
    private:
        std::unordered_map<int, Builder> builders;
};

#endif