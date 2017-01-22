#ifndef COLORS_H
#define COLORS_H

#include <stdbool.h>

#define _COLORS_COUNT 13

typedef struct colors
{
    bool usage[_COLORS_COUNT];
    const int capacity;
} colors;

int colors_init(colors *p);

int colors_use_random(colors *p);

int colors_is_in_use(colors *p, int n);
int colors_set_in_use(colors *p, int n, int value);

#endif /* COLORS_H */

