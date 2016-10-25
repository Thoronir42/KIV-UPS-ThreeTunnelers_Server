#ifndef TANK_H
#define TANK_H

typedef struct tank {
	int x, y;
	int direction;
	int hitpoints;
	int energy;
} tank;

tank *tank_create(int x, int y, int energy, int hitpoints);

void tank_delete(tank *p_tank);

#endif /* TANK_H */