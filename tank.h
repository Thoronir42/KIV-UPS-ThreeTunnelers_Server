#ifndef TANK_H
#define	TANK_H

#define TANK_MAX_HEALTH 20
#define TANK_MAX_ENERGY 20

typedef struct tank {
	int x,
		y;
	int health;
	int energy;
} tank;

#endif	/* TANK_H */

tank *tank_create(int x, int y);

void tank_delete(tank *p_tank);