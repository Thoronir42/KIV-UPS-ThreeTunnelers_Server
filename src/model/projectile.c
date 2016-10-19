#include <stdlib.h>

#include "projectile.h"

projectile *projectile_create(int x, int y, int direction) {
	projectile* p_proj = malloc(sizeof(projectile));
	
	p_proj->x = x;
	p_proj->y = y;
	p_proj->direction = direction;
}

void projectile_delete(projectile *p_proj) {
	free(p_proj);
}