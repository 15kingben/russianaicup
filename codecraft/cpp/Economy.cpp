#include "Economy.hpp"
#include "Util.hpp"
#include <memory>
#include <string>
#include <iostream>


Economy::Economy() {
    resources = 0;
    resourcePerTurn = 0;
    population = 0;
    usedPopulation = 0;
}

int Economy::getResources() {
    return resources;
}

int Economy::getResourcePerTurn() {
    return resourcePerTurn;
}

void Economy::setResources(int resource) {
    resources = resource;
}

bool Economy::charge(EntityType entityType, int cost) {
    int pop = Util::entityProperties[entityType].populationUse;
    if (resources - cost >= 0 && population - pop >= 0) {
        resources -= cost;
        population -= pop;
        return true;
    }
    return false;
}

bool Economy::charge(EntityType entityType) {
    return charge(entityType, Util::entityProperties[entityType].initialCost);
}


void Economy::updateResourcePerTurn() {
    resourcePerTurn += 1;
}

void Economy::setPopulation(int pop) {
    population = pop;
    usedPopulation = 0;
}

void Economy::updatePopulation(Entity entity) {
    EntityType entityType = entity.entityType;
    if (entity.health == Util::entityProperties[entityType].maxHealth)
        population += Util::entityProperties[entityType].populationProvide;
    usedPopulation += Util::entityProperties[entityType].populationUse;
}

int Economy::getPopulation() {
    return population;
}

int Economy::getUsedPopulation() {
    return usedPopulation;
}