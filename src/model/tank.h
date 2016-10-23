#ifndef TANK_H
#define TANK_H

typedef struct tank {
	int x, y;
	int direction;
	int health;
	int energy;
} tank;

tank *tank_create(int x, int y);

void tank_delete(tank *p_tank);

#endif /* TANK_H */