#ifndef ENTITY_SHAPE_H
#define ENTITY_SHAPE_H

#include <stdbool.h>

#include "../model/direction.h"
#include "../model/intdimension.h"

enum shape_type
{
    SHAPE_TANK_BODY, SHAPE_TANK_BELT, SHAPE_PROJECTILE
};

typedef struct shape
{
    intdimension size;
    bool pixels[49]; // 7 * 7 is largest piece,  I AM SORRY #2
    intpoint min, max;
} shape;

void shapes_initialise();

shape shape_get(enum direction direction, enum shape_type);

int shape_is_solid(shape p, int x, int y);

int shape_is_solid_o(shape p, int sx, int sy);
#endif /* ENTITY_SHAPE_H */

