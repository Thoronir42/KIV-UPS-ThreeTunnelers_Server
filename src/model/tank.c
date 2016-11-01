#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "tank.h"

int tank_init(tank *p, int x, int y, int energy, int hitpoints){
	memset(p, 0, sizeof(tank));
	p->location.x = x;
	p->location.y = y;
	
	p->energy = energy;
	p->hitpoints = hitpoints;
	
	return 0;
}