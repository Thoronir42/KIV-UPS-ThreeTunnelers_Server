#include <stdbool.h>
#include "colors.h"

int colors_init(colors *p) {
    int i;
    *(int *) &p->capacity = _COLORS_COUNT;
    for (i = 0; i < p->capacity; i++) {
        p->usage[i] = false;
    }

    return 0;
}

int colors_is_in_use(colors *p, int n) {
    if (n < 0 || n > p->capacity) {
        return -1;
    } else {
        return p->usage[n];
    }
}

int colors_set_in_use(colors *p, int n, int value) {
    bool val = value ? true : false;
    if (n < 0 || n > p->capacity) {
        return -1;
    }
    p->usage[n] = val;

    return 0;

}