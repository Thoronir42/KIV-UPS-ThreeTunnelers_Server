#include <string.h>

#include "warzone.h"
#include "../generic.h"
#include "../model/direction.h"
#include "../logger.h"

warzone_rules _warzone_rules(){
    warzone_rules rules;
    rules.TANK_MAX_EP = 250;
    rules.TANK_MAX_HP = 20;
    rules.MAX_PROJECTILES_PER_TANK = WARZONE_PROJECTILES_PER_TANK;
    rules.TANK_CANNON_COOLDOWN = 5;
    rules.TANK_CANNON_COOLDOWN_RATE = 1;
    
    return rules;
}

void warzone_init(warzone *p, int tanks_size) {
    *(warzone_rules *)&p->rules = _warzone_rules();
    memset(p->tanks, 0, sizeof (tank) * WARZONE_MAX_PLAYERS);
    memset(p->projectiles, 0, sizeof (projectile) * WARZONE_MAX_PROJECTILES);

    p->map_change_count = 0;

    p->tanks_size = tanks_size;
    p->projectiles_size = WARZONE_MAX_PROJECTILES;
}

tank *warzone_get_tank(warzone *p, int playerRID) {
    if (playerRID < 0 || playerRID >= p->tanks_size) {
        return NULL;
    }

    return p->tanks + playerRID;
}

void warzone_init_tank(warzone *p, int player_rid, intpoint location) {
    tank_init(p->tanks + player_rid, location.x, location.y, DIRECTION_N,
            p->rules.TANK_MAX_HP, p->rules.TANK_MAX_EP);
}

int warzone_set_block(warzone *p, int x, int y, block b) {
    struct block_diff diff;
    
    int changed = tunneler_map_set_block(&p->map, x, y, b);
    if(!changed){
        return p->map_change_count;
    }
    if(p->map_change_count + 1 >= WARZONE_BLOCK_DIFF_BUFFER_SIZE){
        glog(LOG_ERROR, "Warzone block diff buffer overflow");
    }
    
    diff.x = x;
    diff.y = y;
    diff.b = b;
    p->map_change_buffer[p->map_change_count] = diff;

    return ++p->map_change_count;
}