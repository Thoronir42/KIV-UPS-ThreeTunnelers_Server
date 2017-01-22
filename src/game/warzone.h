#ifndef WARZONE_H
#define WARZONE_H

#include "../map/tunneler_map.h"
#include "../model/tank.h"
#include "../model/projectile.h"

#define WARZONE_MAX_PLAYERS MAP_MAX_PLAYERS
#define WARZONE_PROJECTILES_PER_TANK 20
#define WARZONE_MAX_PROJECTILES WARZONE_MAX_PLAYERS * WARZONE_PROJECTILES_PER_TANK

struct map_change {
    int x, y;
    block new_block;
};
typedef struct warzone_rules {
    int TANK_MAX_HP;
    int TANK_MAX_EP;
    int MAX_PROJECTILES_PER_TANK;
    int TANK_CANNON_COOLDOWN;
    int TANK_CANNON_COOLDOWN_RATE;
} warzone_rules;

typedef struct warzone {
    const warzone_rules rules;
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

