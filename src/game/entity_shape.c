#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "entity_shape.h"

const bool _g_shape_tank_body_upright[9] = {
    true, true, true,
    true, true, true,
    true, true, true
};

const bool _g_shape_tank_body_diagonal[25] = {
    false, false, true, false, false,
    false, true, true, true, false,
    true, true, true, true, true,
    false, true, true, true, false,
    false, false, true, false, false
};

const bool _g_shape_tank_belt_n[49] = {
    true, false, false, false, false, false, true,
    true, false, false, false, false, false, true,
    true, false, false, false, false, false, true,
    true, false, false, false, false, false, true,
    true, false, false, false, false, false, true,
    true, false, false, false, false, false, true,
    true, false, false, false, false, false, true
};

const bool _g_shape_tank_belt_ne[49] = {
    false, false, false, true, false, false, false,
    false, false, true, false, false, false, false,
    false, true, false, false, false, false, false,
    true, false, false, false, false, false, true,
    false, false, false, false, false, true, false,
    false, false, false, false, true, false, false,
    false, false, false, true, false, false, false
};

const bool _g_shape_projectile_n[3] = {
    true,
    true,
    true
};
const bool _g_shape_projectile_e[3] = {
    true, true, true
};
const bool _g_shape_projectile_ne[9] = {
    false, false, true,
    false, true, false,
    true, false, false
};

const bool _g_shape_tank_belt_e[49];
const bool _g_shape_tank_belt_se[49];

const bool _g_shape_projectile_se[9];

void _shape_rotate(shape *dst, shape *src) {
    int i, j;
    bool value;
    dst->size.height = src->size.width;
    dst->size.width = src->size.height;

    for (j = 0; j < dst->size.height; j++) {
        for (i = 0; i < dst->size.width; i++) {
            value = src->pixels[j * src->size.width + i];
            dst->pixels[i * dst->size.width + dst->size.width - 1 - j] = value;
        }
    }


}

void shapes_initialise() {
    shape tmp_src, tmp_dst;

    tmp_src = shape_get(DIRECTION_N, SHAPE_TANK_BELT);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_belt_e, tmp_dst.pixels, 49 * sizeof (bool));

    tmp_src = shape_get(DIRECTION_NE, SHAPE_TANK_BELT);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_belt_se, tmp_dst.pixels, 49 * sizeof (bool));

    tmp_src = shape_get(DIRECTION_NE, SHAPE_PROJECTILE);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_projectile_se, tmp_dst.pixels, 9 * sizeof (bool));
}

shape _shape_get_body(enum direction direction) {
    shape tmp;
    switch (direction) {
        case DIRECTION_N: case DIRECTION_E:
        case DIRECTION_S: case DIRECTION_W:
            tmp.size.width = tmp.size.height = 3;
            memcpy(tmp.pixels, _g_shape_tank_body_upright, 9 * sizeof (bool));
            return tmp;
        case DIRECTION_NE: case DIRECTION_SE:
        case DIRECTION_SW: case DIRECTION_NW:
            tmp.size.width = tmp.size.height = 5;
            memcpy(tmp.pixels, _g_shape_tank_body_diagonal, 25 * sizeof (bool));
            return tmp;
    }
    tmp.size.width = tmp.size.height = 0;
    return tmp;
}

shape _shape_get_belt(enum direction direction) {
    shape tmp;
    tmp.size.width = tmp.size.height = 7;
    switch (direction) {
        case DIRECTION_N: case DIRECTION_S:
            memcpy(tmp.pixels, _g_shape_tank_belt_n, 49 * sizeof (bool));
            return tmp;
        case DIRECTION_E: case DIRECTION_W:
            memcpy(tmp.pixels, _g_shape_tank_belt_e, 49 * sizeof (bool));
            return tmp;
        case DIRECTION_NE: case DIRECTION_SW:
            memcpy(tmp.pixels, _g_shape_tank_belt_ne, 49 * sizeof (bool));
            return tmp;
        case DIRECTION_SE: case DIRECTION_NW:
            memcpy(tmp.pixels, _g_shape_tank_belt_se, 49 * sizeof (bool));
            return tmp;
    }
    tmp.size.width = tmp.size.height = 0;
    return tmp;
}

shape _shape_get_projectile(enum direction direction) {
    shape tmp;
    tmp.size.width = tmp.size.height = 3;
    switch (direction) {
        case DIRECTION_N: case DIRECTION_S:
            tmp.size.width = 1;
            memcpy(tmp.pixels, _g_shape_projectile_n, 3 * sizeof (bool));
            return tmp;
        case DIRECTION_E: case DIRECTION_W:
            tmp.size.height = 1;
            memcpy(tmp.pixels, _g_shape_projectile_e, 3 * sizeof (bool));
            return tmp;
        case DIRECTION_NE: case DIRECTION_SW:
            memcpy(tmp.pixels, _g_shape_projectile_ne, 9 * sizeof (bool));
            return tmp;
        case DIRECTION_SE: case DIRECTION_NW:
            memcpy(tmp.pixels, _g_shape_projectile_se, 9 * sizeof (bool));
            return tmp;
    }
    tmp.size.width = tmp.size.height = 0;
    return tmp;
}

shape shape_get(enum direction direction, enum shape_type type) {
    switch (type) {
        case SHAPE_TANK_BODY:
            return _shape_get_body(direction);
        case SHAPE_TANK_BELT:
            return _shape_get_belt(direction);
        case SHAPE_PROJECTILE:
            return _shape_get_projectile(direction);
    }
}

int shape_is_solid(shape p, int x, int y) {
    int offset = y * p.size.width + x;
    return (int) p.pixels[offset];
}
