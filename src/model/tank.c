#include <stddef.h>
#include <stdlib.h>

#include "tank.h"

tank *tank_create(int x, int y, int energy, int hitpoints){
	tank *tmp = malloc(sizeof(tank));
	tmp->x = x;
	tmp->y = y;
	
	tmp->energy = energy;
	tmp->hitpoints = hitpoints;
	
	return tmp;
}

void tank_delete(tank *p_tank){
	free(p_tank);
}