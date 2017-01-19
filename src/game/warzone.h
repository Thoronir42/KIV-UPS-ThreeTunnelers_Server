#ifndef WARZONE_H
#define WARZONE_H

#include "../map/tunneler_map.h"
#include "../model/tank.h"
#include "../model/projectile.h"

#define WARZONE_MAX_PLAYERS MAP_MAX_PLAYERS
#define WARZONE_PROJECTILES_PER_TANK 20
#define WARZONE_MAX_PROJECTILES WARZONE_MAX_PLAYERS * WARZONE_PROJECTILES_PER_TANK

typedef struct warzone {
    const int TANK_MAX_HP,
    TANK_MAX_EP;
    tunneler_map map;

    tank tanks[WARZONE_MAX_PLAYERS];
    int tanks_size;
    projectile projectiles[WARZONE_MAX_PROJECTILES];
    int projectiles_size;

    int current_round;
    int total_rounds;


} warzone;

void warzone_init(warzone *p, int tanks_size);

tank *warzone_get_tank(warzone *p, int playerRID);

#endif /* WARZONE_H */

