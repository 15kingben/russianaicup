#include "ConstructManager.hpp"
#include "BuilderManager.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "Repairable.hpp"

const int STEP_TIMEOUT = 60;

Repairable::Repairable() {

}

Job::Job() {
    step = 0;
}

EntityAction Job::getAction(std::vector<std::vector<Square> > & open) {
    if (actions.empty() || step > STEP_TIMEOUT) {
        return Util::getEmptyAction();
    }
    step++;
    EntityAction next = actions.front(); actions.pop_front();
    std::cout << entity.id << " Executing job action: " << std::endl;
    Util::printAction(next);

    if (next.moveAction != nullptr) {
        if (entity.position.x == next.moveAction.get()->target.x && entity.position.y == next.moveAction.get()->target.y) {
            std::cout << "WERE HERE" << std::endl;
            std::cout << Util::printVec(entity.position) << std::endl;
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
        BuildAction buildAction = *next.buildAction.get();
        Vec2Int b = Util::getBuildPosition(buildAction.position, buildAction.entityType, open);

        std::unordered_set<int> obsctructions = Util::getClear(*next.buildAction, open);
        if (!obsctructions.empty()) { // in future, code removal of resources and enemy builders
            for (auto x : obsctructions) std::cout << x << std::endl;
            std::cout << "WAIT" << std::endl;
            actions.push_front(next);
            return Util::getAction(MoveAction(entity.position, false, false));
                    
            //         EntityAction getOutOfTheWay = Util::getAction(MoveAction(b,false,false));

            // for (auto obs : obsctructions) {
            // // clear out any enemies or resources which are in the way
            //     std::cout << "CLEARING " << target.id << " " << Util::printEntityType(target.entityType) << std::endl;
            //     actions.push_front(next);
            //     // if the obstruction is friendly, just wait
            //     if (target.playerId && *target.playerId.get() == Util::myId) {
                    
            //     }
            // }
            // return Util::getAction(Util::getAttackAction(std::make_shared<int>(target.id), 5, std::vector<EntityType>({target.entityType})));
        }

        // std::unordered_set<Vec2Int> poop = Util::getNeighborPositions(buildAction.position, buildAction.entityType);
        // bool isNeighbor = false;
        // for (auto x : poop) {
        //     if (x == entity.position) {
        //         isNeighbor = true;
        //         break;
        //     }

        // }

        if (!Util::isNeighbor(entity.position, Entity(-1,0,buildAction.entityType, buildAction.position, 0, false))) {
            std::cout << "DETOUR" << std::endl;
            // we must have just detoured to clear the area, return back to build location
            actions.push_front(next);
            return Util::getAction(MoveAction(b, false, true));
        }
        if (!Util::economy->charge(next.buildAction->entityType)) {
            std::cout << "NO MONEY " << Util::economy->getResources() << std::endl;
            // hold in place
            actions.push_front(next);
            return Util::getAction(MoveAction(entity.position, false, false));
        }

        std::cout<<"BUILDING NOW" << std::endl;
        return next;
    }

    if (next.repairAction != nullptr) {
        int targetId = next.repairAction.get()->target;
        Entity target;
        if (Util::entities.find(targetId) == Util::entities.end()) return Util::getEmptyAction();
        target = Util::entities[targetId];
        if (target.health == Util::entityProperties[target.entityType].maxHealth) {
            // done
            return Util::getEmptyAction();
        }
        // push front since not full hp
        actions.push_front(next);

        // we have our target, but we may need to pathfind first
        if (!Util::isNeighbor(entity.position, target)) {
            return Util::getAction(MoveAction(Util::getBuildPosition(target.position, target.entityType, open), false, true));
        }
 
        return next;
    }

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
    std::vector<EntityType> mineTargets({RESOURCE});
    if (Util::economy->getPopulation() == 5) {
        mineTargets.push_back(BUILDER_UNIT);
    }
    return Util::getAction(AttackAction(nullptr, std::make_shared<AutoAttack>(AutoAttack(1000, mineTargets))));
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
            // actions[pair.first] = getMineAction();
            actions[pair.first] = Util::getAction(RepairAction(-1));
        }
    }
}

int BuilderManager::getClosestWorker(Vec2Int location) {
    int minDistance = 100000;
    int workerId = -1;
    for (auto & pair : builders) {
        if (pair.second.committed) continue;
        if (Util::dist2(location, pair.second.entity.position) < minDistance) {
            minDistance = Util::dist2(location, pair.second.entity.position);
            workerId = pair.first;
        }
    }
    return workerId;
}

int BuilderManager::assignNearestWorkerToBuild(Vec2Int location, EntityType type, std::vector<std::vector<Square> > & open) {
    int workerId = getClosestWorker(location);
    if (workerId == -1) return -1;

    // Give the worker the tasks
    Job job;
    // job.actions.push_front(Util::getAction(RepairAction(-1)));
    job.actions.push_front(Util::getAction(BuildAction(type, location)));
    job.actions.push_front(Util::getAction(MoveAction(Util::getBuildPosition(location, type, open), false, true)));
    job.entity = Util::entities[workerId];
    builders[workerId].committed = true;
    builders[workerId].job = job;
    
    return workerId;
}

int BuilderManager::assignNearestWorkerToRepair(Entity entity) {
    int workerId = getClosestWorker(entity.position);
    if (workerId == -1) return -1;

    // Give the worker the tasks
    Job job;
    // job.actions.push_front(Util::getAction(RepairAction(-1)));
    job.actions.push_front(Util::getAction(RepairAction(entity.id)));
    job.entity = Util::entities[workerId];
    builders[workerId].committed = true;
    builders[workerId].job = job;
    

    return workerId;
}

int BuilderManager::getCommitted() {
    int s = 0;
    for (auto pair : builders) {
        if (pair.second.committed) s++;
    }
    return s;
}

int MAX_REPAIR_CT = 2;

void BuilderManager::repair(Repairable& entity, std::vector<std::vector<Square> > & open) {
    for (int i = 0; i < 5; i++) {
        std::cout << entity.getEntity().entityType << std::endl;
    }
    if (entity.getEntity().health == Util::entityProperties[entity.getEntity().entityType].maxHealth) {
        entity.helpers.clear();
        return;
    }

    int x = (getBuilderCount() / 5) - getCommitted();
    for (int helper : entity.helpers) {
        if (Util::entities.find(helper) == Util::entities.end()) {
            entity.helpers.erase(helper);
        }
    }

    while (x > 0 && entity.helpers.size() < MAX_REPAIR_CT) {
        int id = assignNearestWorkerToRepair(entity.getEntity());
        if (id != -1) {
            entity.helpers.emplace(id);
            x--;
        }
    }
}