#ifndef _ECONOMY_HPP_
#define _ECONOMY_HPP_

class Economy {
public:
    Economy();
    int getResources();
    int getResourcePerTurn();
    void setResources(int resource);
    bool charge(int cost);
    void updateResourcePerTurn();
private:
    int resourcePerTurn;
    int resources;
};

#endif