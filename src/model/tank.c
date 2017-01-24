#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "tank.h"
#include "direction.h"

void tank_init(tank *p, int x, int y, enum direction direction, int hitpoints, int energy) {
    p->status = TANK_STATUS_OPERATIVE;
    p->location.x = x;
    p->location.y = y;
    p->direction = direction;
    p->energy = energy;
    p->hitpoints = hitpoints;
    p->cooldown = 0;
}

int tank_reduce_hitpoints(tank *p) {
    if (p->hitpoints > 0) {
        p->hitpoints--;
    }
    return p->hitpoints;
}

int tank_reduce_energy(tank *p) {
    if (p->energy > 0) {
        p->energy--;
    }

    return p->energy;
}
