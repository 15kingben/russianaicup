#ifndef _SQUARE_HPP_
#define _SQUARE_HPP_

#include "model/Model.hpp"

class Square {
public:
    Square();
    Square(Entity entity);
    void setEntity(Entity entity);
    bool isOccupied();
    void setPlanned();
    Entity getEntity();

   bool operator<(const Square& rhs) const noexcept
   {
      // logic here
      return this < &rhs; // for example
   }
private:
    bool occupied;
    Entity entity;
};

#endif