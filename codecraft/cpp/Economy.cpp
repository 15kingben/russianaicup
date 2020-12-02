#include "Economy.hpp"
#include <memory>
#include <string>



Economy::Economy() {
    resources = 0;
    resourcePerTurn = 0;
}

int Economy::getResources() {
    return resources;
}

int Economy::getResourcePerTurn() {
    return resourcePerTurn;
}

void Economy::setResources(int resource) {
    resources = resources;
}

bool Economy::charge(int cost) {
    if (resources - cost >= 0) {
        resources -= cost;
        return true;
    }
    return false;
}

void Economy::updateResourcePerTurn() {
    resourcePerTurn += 1;
}