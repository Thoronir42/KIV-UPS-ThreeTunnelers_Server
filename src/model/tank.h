#ifndef TANK_H
#define TANK_H

#include "intpoint.h"

typedef struct tank {
    intpoint location;

    int direction;
    int hitpoints;
    int energy;

    int player_rid;
} tank;

void tank_init(tank *p, int x, int y, int direction, int energy, int hitpoints);


#endif /* TANK_H */