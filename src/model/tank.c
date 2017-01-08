#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "tank.h"

void tank_init(tank *p, int x, int y, int direction, int energy, int hitpoints) {
    p->location.x = x;
    p->location.y = y;
    p->direction = direction;
    p->energy = energy;
    p->hitpoints = hitpoints;
}