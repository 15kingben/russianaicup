#include "Economy.hpp"
#include "Util.hpp"
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
    resources = resource;
}

bool Economy::charge(EntityType entityType) {
    int cost = Util::entityProperties[entityType].cost;
    int pop = Util::entityProperties[entityType].populationUse;
    if (resources - cost >= 0 && population - pop >= 0) {
        resources -= cost;
        population -= pop;
        return true;
    }
    return false;
}

void Economy::updateResourcePerTurn() {
    resourcePerTurn += 1;
}

void Economy::setPopulation(int pop) {
    population = pop;
}

void Economy::updatePopulation(EntityType entityType) {
    population += Util::entityProperties[entityType].populationProvide;
    population -= Util::entityProperties[entityType].populationUse;
}

int Economy::getPopulation() {
    return population;
}