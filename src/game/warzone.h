#ifndef WARZONE_H
#define WARZONE_H

#include "../map/tunneler_map.h"
#include "block_diff.h"

#include "../model/tank.h"
#include "../model/projectile.h"


#define WARZONE_MAX_PLAYERS MAP_MAX_PLAYERS
#define WARZONE_PROJECTILES_PER_TANK 20
#define WARZONE_MAX_PROJECTILES WARZONE_MAX_PLAYERS * WARZONE_PROJECTILES_PER_TANK
// Edge case never to happen is that 49 changes will occur per tank move
#define WARZONE_BLOCK_DIFF_BUFFER_SIZE 50 * WARZONE_MAX_PLAYERS

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
    
    struct block_diff map_change_buffer[WARZONE_BLOCK_DIFF_BUFFER_SIZE];
    int map_change_count;
    
    
    tank tanks[WARZONE_MAX_PLAYERS];
    int tanks_size;
    projectile projectiles[WARZONE_MAX_PROJECTILES];
    int projectiles_size;
    
    int current_round;
    int total_rounds;


} warzone;

void warzone_init(warzone *p, int tanks_size);

void warzone_init_tank(warzone *p, int player_rid, intpoint location);

tank *warzone_get_tank(warzone *p, int playerRID);

int warzone_set_block(warzone *p, int x, int y, block b);

#endif /* WARZONE_H */

