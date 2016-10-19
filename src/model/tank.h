#ifndef TANK_H
#define	TANK_H

const int TANK_MAX_HEALTH,
	TANK_MAX_ENERGY;

typedef struct tank {
	int x,
		y;
	int direction;
	int health;
	int energy;
} tank;

tank *tank_create(int x, int y);

void tank_delete(tank *p_tank);

#endif	/* TANK_H */