#include <stdlib.h>

#include "projectile.h"

projectile *projectile_create(int x, int y, int direction) {
    projectile* p_proj = malloc(sizeof (projectile));

    p_proj->location.x = x;
    p_proj->location.y = y;
    p_proj->direction = direction;
}