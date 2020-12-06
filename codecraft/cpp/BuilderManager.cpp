#include "ConstructManager.hpp"
#include "BuilderManager.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

Job::Job() {

}

EntityAction Job::getAction(std::vector<std::vector<Square> > & open) {
    EntityAction next = actions.front(); actions.pop_front();
    std::cout << entity.id << " Executing job action: " << std::endl;
    Util::printAction(next);

    if (next.moveAction != nullptr) {
        if (entity.position.x == next.moveAction.get()->target.x && entity.position.y == next.moveAction.get()->target.y) {
            std::cout << "WERE HERE" << std::endl;
            // we're done moving, on to building
            next = actions.front();
            actions.pop_front();
        } else {
                        std::cout << "NOT HERE" << std::endl;

            actions.push_front(next);
            return next;
        }
    }

    if (next.buildAction != nullptr) {
        Entity target = Util::getClear(*next.buildAction, open);
        if (target.id != -1) {
            // clear out any enemies or resources which are in the way
            std::cout << "CLEARING " << target.id << " " << Util::printEntityType(target.entityType) << std::endl;
            actions.push_front(next);
            // if the obstruction is friendly, just wait
            if (target.playerId && *target.playerId.get() == Util::myId) {
                return Util::getAction(MoveAction(entity.position, false, false));
            }

            return Util::getAction(Util::getAttackAction(std::make_shared<int>(target.id), 15, std::vector<EntityType>({target.entityType})));
        }

        BuildAction buildAction = *next.buildAction.get();

        Vec2Int b = Util::getBuildPosition(buildAction.position, buildAction.entityType, open);
        if (entity.position.x != b.x || entity.position.y != b.y) {
            std::cout << "DETOUR" << std::endl;
            // we must have just detoured to clear the area, return back to build location
            actions.push_front(next);
            return Util::getAction(MoveAction(b, false, true));
        }
        if (!Util::economy.charge(next.buildAction->entityType)) {
            std::cout << "NO MONEY" << std::endl;
            // hold in place
            actions.push_front(next);
            return Util::getAction(MoveAction(entity.position, false, false));
        }

        return next;
    }

    std::cout << "poop" << std::endl;

    if (next.repairAction != nullptr) {
        int targetId = next.repairAction.get()->target;
        Entity target;
        if (targetId == -1) {
            // we did not specify a target prior, so repair the thing we just built
            target = open[entity.position.x+1][entity.position.y].getEntity();
            if (target.id == -1) {
                // give up
                return Util::getEmptyAction();
            }
        } else {
            target = Util::entities[targetId];
            // we have our target, but we may need to pathfind first
            if (!Util::isAdjacent(entity.position, target.position)) {
                actions.push_front(next);
                actions.push_front(Util::getAction(MoveAction(Vec2Int(target.position.x-1,target.position.y), false, true)));
            }
        }
        
        if (target.health == Util::entityProperties[target.entityType].maxHealth) {
            // done
            return Util::getEmptyAction();
        }

        return next;
    }

    std::cout << "poop" << std::endl;

    return Util::getEmptyAction();
}

BuilderManager::BuilderManager() {
    
}

Builder::Builder(Entity entity, Role role) {
    this->entity = entity;
    this->role = role;
    committed = false;
}

Builder::Builder() {

}

int BuilderManager::getBuilderCount() const {
    return builders.size();
}

void BuilderManager::updateBuilders(const std::unordered_map<int, Entity> & currentBuilders) {
    // Remove dead builders from the list
    for (auto it = builders.begin(); it != builders.end();) {
        if (currentBuilders.find(it->first) == currentBuilders.end()) {
            it = builders.erase(it);
        } else it++;
    }

    // Add new builders (all miners for now)
    for (auto pair : currentBuilders) {
        if (builders.find(pair.first) != builders.end()) {
            builders[pair.first].entity = pair.second;
        } else {
            builders[pair.first] = Builder(pair.second, MINE);
        }
    }

    // yuck
    for (auto & pair : builders) {
        pair.second.job.entity = pair.second.entity;
    }
}

EntityAction getMineAction() {
    return Util::getAction(AttackAction(nullptr, std::make_shared<AutoAttack>(AutoAttack(1000, std::vector<EntityType>({RESOURCE})))));
}

void BuilderManager::builderActions(std::unordered_map<int, EntityAction> & actions, std::vector<std::vector<Square> > & open) {
    for (auto & pair : builders) {
        if (pair.second.committed) {
            // we have an active job
            EntityAction action = pair.second.job.getAction(open);
            if (action.attackAction == nullptr && action.buildAction == nullptr && action.moveAction == nullptr && action.repairAction == nullptr) {
                // done with job
                std::cout << "Done with job" << std::endl;
                pair.second.committed = false;
                pair.second.job = Job();
                actions[pair.first] = getMineAction();
            } else {
                actions[pair.first] = action;
            }
        } else {
            actions[pair.first] = getMineAction();
        }
    }
}

int BuilderManager::assignNearestWorkerToBuild(Vec2Int location, EntityType type, std::vector<std::vector<Square> > & open) {
    int minDistance = 100000;
    int workerId = -1;
    for (auto & pair : builders) {
        if (pair.second.committed) continue;
        if (Util::dist2(location, pair.second.entity.position) < minDistance) {
            minDistance = Util::dist2(location, pair.second.entity.position);
            workerId = pair.first;
        }
    }

    // Give the worker the tasks
    Job job;
    job.actions.push_front(Util::getAction(RepairAction(-1)));
    job.actions.push_front(Util::getAction(BuildAction(type, location)));
    job.actions.push_front(Util::getAction(MoveAction(Util::getBuildPosition(location, type, open), false, false)));
    builders[workerId].committed = true;
    builders[workerId].job = job;

    return workerId;
}