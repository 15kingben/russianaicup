#ifndef _BUILDER_MANAGER_HPP_
#define _BUILDER_MANAGER_HPP_

#include "Square.hpp"
#include <unordered_map>
#include "model/Model.hpp"
#include <deque>
#include "Job.hpp"
#include "Builder.hpp"
#include "Repairable.hpp"

class BuilderManager {
    public:
        BuilderManager();
        int getBuilderCount() const;
        void updateBuilders(const std::unordered_map<int, Entity> & currentBuilders);
        void builderActions(std::unordered_map<int, EntityAction> & actions, std::vector<std::vector<Square> > & open);
        std::unordered_map<int, Builder> builders;
        int assignNearestWorkerToBuild(Vec2Int location, EntityType type, std::vector<std::vector<Square> > & open);
        int assignNearestWorkerToRepair(Entity entity);
        void repair(Repairable& entity, std::vector<std::vector<Square> > & open);
        int getCommitted();
        int getClosestWorker(Vec2Int location) ;
};

#endif