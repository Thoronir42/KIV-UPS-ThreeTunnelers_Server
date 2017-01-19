#include <string.h>

#include "warzone.h"

void warzone_init(warzone *p) {
    memset(p->tanks, 0, sizeof (tank) * WARZONE_MAX_PLAYERS);
    memset(p->projectiles, 0, sizeof (projectile) * WARZONE_MAX_PROJECTILES);
    
    tunneler_map_init(&p->map, 8, 6, 21);
}