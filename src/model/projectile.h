#ifndef PROJECTILE_H
#define PROJECTILE_H

typedef struct projectile {
	int x,
		y;
	int direction;
} projectile;

projectile *projectile_create(int x, int y, int direction);

void projectile_delete(projectile *p_proj);

#endif /* PROJECTILE_H */

