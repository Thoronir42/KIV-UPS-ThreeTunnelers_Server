#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "intpoint.h"

typedef struct projectile
{
    intpoint location;
    int direction;

    unsigned char player_rid;
} projectile;

void projectile_init(projectile *p, int x, int y, int direction, int player_rid);

void projectile_clear(projectile *p);

#endif /* PROJECTILE_H */

