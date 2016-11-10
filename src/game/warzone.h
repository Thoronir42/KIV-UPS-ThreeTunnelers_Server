#ifndef WARZONE_H
#define WARZONE_H

#include "../map/tunneler_map.h"
#include "../model/tank.h"
#include "../model/projectile.h"

typedef struct warzone {
    const int TANK_MAX_HP,
    TANK_MAX_EP;
    tunneler_map *map;

    tank *tanks;
    int tanks_size;
    projectile *projectiles;
    int projectiles_size;

    int current_round;
    int total_rounds;


} warzone;

#endif /* WARZONE_H */

