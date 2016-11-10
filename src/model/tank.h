#ifndef TANK_H
#define TANK_H

#include "../data/intpoint.h"

typedef struct tank {
    intpoint location;

    int direction;
    int hitpoints;
    int energy;

    int player_rid;
} tank;

int tank_init(tank *p, int x, int y, int energy, int hitpoints);



#endif /* TANK_H */