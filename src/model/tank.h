#ifndef TANK_H
#define TANK_H

#include "direction.h"
#include "intpoint.h"

typedef enum tank_status
{
    TANK_STATUS_EMPTY = 0, TANK_STATUS_OPERATIVE = 1,
    TANK_STATUS_DESTROYED = 2,

} tank_status;

typedef struct tank
{
    tank_status status;
    intpoint location;

    enum direction direction;
    int hitpoints;
    int energy;
    
    int cooldown;
} tank;

void tank_init(tank *p, int x, int y, enum direction direction, int hitpoints, int energy);


#endif /* TANK_H */