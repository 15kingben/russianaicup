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
    if (next.moveAction != nullptr) {
        if (entity.position.x == next.moveAction.get()->target.x && entity.position.y == next.moveAction.get()->target.y) {
            // we're done moving, on to building
            next = actions.front();
            actions.pop_front();
        } else {
            actions.push_front(next);
            return next;
        }
    }

    if (next.buildAction != nullptr) {
        Entity target = Util::getClear(*next.buildAction, open);
        if (target.id != -1) {
            // clear out any enemies or resources which are in the way
            actions.push_front(next);
            return Util::getAction(Util::getAttackAction(std::make_shared<int>(target.id), 15, std::vector<EntityType>({target.entityType})));
        }
        int bx = next.buildAction->position.x - 1;
        int by = next.buildAction->position.y;
        if (entity.position.x != bx || entity.position.y != by) {
            // we must have detoured to clear the area, return back to build location
            actions.push_front(next);
            actions.push_front(Util::getAction(MoveAction(Vec2Int(bx,by), false, true)));
        }
        if (!Util::economy.charge(next.buildAction->entityType)) {
            // hold in place
            actions.push_front(next);
            return Util::getAction(MoveAction(entity.position, false, false));
        }

        return next;
    }

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
    for (auto pair : builders) {
        pair.second.job.entity = pair.second.entity;
    }
}

void BuilderManager::builderActions(std::unordered_map<int, EntityAction> & actions) {
    for (auto pair : builders) {
        actions[pair.first] = Util::getAction(AttackAction(nullptr, std::make_shared<AutoAttack>(AutoAttack(1000, std::vector<EntityType>({RESOURCE})))));
    }
}
