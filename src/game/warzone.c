#include <string.h>

#include "warzone.h"

void warzone_init(warzone *p, int tanks_size) {
    memset(p->tanks, 0, sizeof (tank) * WARZONE_MAX_PLAYERS);
    memset(p->projectiles, 0, sizeof (projectile) * WARZONE_MAX_PROJECTILES);
    
    p->tanks_size = tanks_size;
    p->projectiles_size = WARZONE_PROJECTILES_PER_TANK;
    
    tunneler_map_init(&p->map, 8, 6, 21);
}

tank *warzone_get_tank(warzone *p, int playerRID){
    if(playerRID < 0 || playerRID >= p->tanks_size){
        return NULL;
    }
    
    return p->tanks + playerRID;
}