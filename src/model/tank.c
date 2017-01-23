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
}
