#ifndef COLORS_H
#define COLORS_H

#include <stdbool.h>

#define _COLORS_COUNT 13

typedef struct colors {
    bool usage[_COLORS_COUNT];
    const int capacity;
} colors;

int colors_init(colors *p);

int colors_is_in_use(colors *p, int n);
void colors_set_in_use(colors *p, int n, bool value);

#endif /* COLORS_H */

