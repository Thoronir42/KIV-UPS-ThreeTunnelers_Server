#include "tank.h"

tank *tank_create(int x, int y){
	tank *tmp = malloc(sizeof(tank));
	tmp->x = x;
	tmp->y = y;
	
	tmp->energy = TANK_MAX_ENERGY;
	tmp->health = TANK_MAX_HEALTH;
	
	return tmp;
}

void tank_delete(tank *p_tank){
	free(p_tank);
}