#ifndef DIRECTION_H
#define DIRECTION_H

#include "intpoint.h"

intpoint G_DIRECTIONS[9];

enum direction
{
    DIRECTION_0 = 0,
    DIRECTION_N = 1,
    DIRECTION_NE = 2,
    DIRECTION_E = 3,
    DIRECTION_SE = 4,
    DIRECTION_S = 5,
    DIRECTION_SW = 6,
    DIRECTION_W = 7,
    DIRECTION_NW = 8,
};

void directions_initialise();

#endif /* INPUT_H */

