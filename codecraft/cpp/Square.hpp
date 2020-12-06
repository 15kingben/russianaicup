#ifndef _SQUARE_HPP_
#define _SQUARE_HPP_

#include "Model.hpp"

class Square {
public:
    Square();
    Square(Entity entity);
    void setEntity(Entity entity);
    bool isOccupied();
    void setPlanned();
    Entity getEntity();
private:
    bool occupied;
    Entity entity;
};

#endif