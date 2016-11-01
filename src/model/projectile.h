#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../data/intpoint.h"

typedef struct projectile {
	intpoint location;
	int direction;
} projectile;

projectile *projectile_create(int x, int y, int direction);

void projectile_delete(projectile *p_proj);

#endif /* PROJECTILE_H */

