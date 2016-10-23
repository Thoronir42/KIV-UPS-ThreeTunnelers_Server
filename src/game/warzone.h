#ifndef WARZONE_H
#define WARZONE_H

#include "../model/tank.h"
#include "../model/projectile.h"

typedef struct warzone {
	tank tanks[];
	projectile projectiles[];
	
	tunneler_map *map;
	
	int total_rounds;
	int current_round;
	
	int tank_max_hp, tank_max_ep;
} warzone;

#endif /* WARZONE_H */

