#include <stdlib.h>
#include <string.h>

#include "projectile.h"

void projectile_init(projectile *p, int x, int y, enum direction direction, int player_rid) {
    p->location.x = x;
    p->location.y = y;
    p->direction = direction;
    p->player_rid = player_rid;
}

void projectile_clear(projectile *p) {
    memset(p, 0, sizeof(projectile));
}