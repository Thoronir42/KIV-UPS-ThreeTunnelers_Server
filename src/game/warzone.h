#ifndef WARZONE_H
#define WARZONE_H

#include "../map/tunneler_map.h"
#include "../model/tank.h"
#include "../model/projectile.h"

typedef struct warzone {
	tunneler_map *map;
	
	tank *tanks;
	int tanks_size;
	projectile *projectiles;
	int projectiles_size;
	
	int total_rounds;
	int current_round;
	
	int tank_max_hp, tank_max_ep;
} warzone;

#endif /* WARZONE_H */

