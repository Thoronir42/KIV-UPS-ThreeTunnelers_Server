#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "entity_shape.h"

const bool _g_shape_tank_body_n[15] = {
    true, true, true,
    true, true, true,
    true, true, true,
    true, true, true,
    false, false, false,
};
const bool _g_shape_tank_body_e[15];
const bool _g_shape_tank_body_s[15];
const bool _g_shape_tank_body_w[15];

const bool _g_shape_tank_body_diagonal[25] = {
    false, false, true, false, false,
    false, true, true, true, false,
    true, true, true, true, true,
    false, true, true, true, false,
    false, false, true, false, false
};

const bool _g_shape_tank_belt_n[35] = {
    false, false, false, false, false,
    true, false, false, false, true,
    true, false, false, false, true,
    true, false, false, false, true,
    true, false, false, false, true,
    true, false, false, false, true,
    true, false, false, false, true
};
const bool _g_shape_tank_belt_e[35];
const bool _g_shape_tank_belt_s[35];
const bool _g_shape_tank_belt_w[35];

const bool _g_shape_tank_belt_ne[49] = {
    false, false, false, true, false, false, false,
    false, false, true, false, false, false, false,
    false, true, false, false, false, false, false,
    true, false, false, false, false, false, true,
    false, false, false, false, false, true, false,
    false, false, false, false, true, false, false,
    false, false, false, true, false, false, false
};
const bool _g_shape_tank_belt_se[49];

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
const bool _g_shape_projectile_se[9];

void _shape_rotate(shape *dst, shape *src) {
    int i, j, dest_offset;
    bool value;
    dst->size.height = src->size.width;
    dst->size.width = src->size.height;

    for (j = 0; j < src->size.height; j++) {
        for (i = 0; i < src->size.width; i++) {
            value = shape_is_solid(*src, i, j) ? true : false;
            dest_offset = (i * dst->size.width) + (dst->size.width - 1 - j);
            dst->pixels[dest_offset] = value;
        }
    }
}

void shapes_initialise() {
    shape tmp_src, tmp_dst;

    tmp_src = shape_get(DIRECTION_N, SHAPE_TANK_BODY);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_body_e, tmp_dst.pixels, 15 * sizeof (bool)); // north to east
    tmp_src = shape_get(DIRECTION_E, SHAPE_TANK_BODY);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_body_s, tmp_dst.pixels, 15 * sizeof (bool)); // east to south
    tmp_src = shape_get(DIRECTION_S, SHAPE_TANK_BODY);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_body_w, tmp_dst.pixels, 15 * sizeof (bool)); // south to wests

    tmp_src = shape_get(DIRECTION_N, SHAPE_TANK_BELT);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_belt_e, tmp_dst.pixels, 35 * sizeof (bool)); // north to east
    tmp_src = shape_get(DIRECTION_E, SHAPE_TANK_BELT);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_belt_s, tmp_dst.pixels, 35 * sizeof (bool)); // east to south
    tmp_src = shape_get(DIRECTION_S, SHAPE_TANK_BELT);
    _shape_rotate(&tmp_dst, &tmp_src);
    memcpy((bool *) _g_shape_tank_belt_w, tmp_dst.pixels, 35 * sizeof (bool)); // south to west



    // rotate diagonal stuffs
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
        case DIRECTION_N:
            tmp.size.height = 5;
            tmp.size.width = 3;
            memcpy(tmp.pixels, _g_shape_tank_body_n, 15 * sizeof (bool));
            return tmp;
        case DIRECTION_E:
            tmp.size.height = 3;
            tmp.size.width = 5;
            memcpy(tmp.pixels, _g_shape_tank_body_e, 15 * sizeof (bool));
            return tmp;
        case DIRECTION_S:
            tmp.size.height = 5;
            tmp.size.width = 3;
            memcpy(tmp.pixels, _g_shape_tank_body_s, 15 * sizeof (bool));
            return tmp;
        case DIRECTION_W:
            tmp.size.height = 3;
            tmp.size.width = 5;
            memcpy(tmp.pixels, _g_shape_tank_body_w, 15 * sizeof (bool));
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
    switch (direction) {
        case DIRECTION_N:
            tmp.size.width = 5;
            tmp.size.height = 7;
            memcpy(tmp.pixels, _g_shape_tank_belt_n, 35 * sizeof (bool));
            return tmp;
        case DIRECTION_S:
            tmp.size.width = 5;
            tmp.size.height = 7;
            memcpy(tmp.pixels, _g_shape_tank_belt_s, 35 * sizeof (bool));
            return tmp;
        case DIRECTION_E:
            tmp.size.width = 7;
            tmp.size.height = 5;
            memcpy(tmp.pixels, _g_shape_tank_belt_e, 35 * sizeof (bool));
            return tmp;
        case DIRECTION_W:
            tmp.size.width = 7;
            tmp.size.height = 5;
            memcpy(tmp.pixels, _g_shape_tank_belt_w, 35 * sizeof (bool));
            return tmp;
        case DIRECTION_NE: case DIRECTION_SW:
            tmp.size.width = tmp.size.height = 7;
            memcpy(tmp.pixels, _g_shape_tank_belt_ne, 49 * sizeof (bool));
            return tmp;
        case DIRECTION_SE: case DIRECTION_NW:
            tmp.size.width = tmp.size.height = 7;
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
    shape tmp;
    switch (type) {
        case SHAPE_TANK_BODY:
            tmp = _shape_get_body(direction);
            break;
        case SHAPE_TANK_BELT:
            tmp = _shape_get_belt(direction);
            break;
        case SHAPE_PROJECTILE:
            tmp = _shape_get_projectile(direction);
            break;
        default:
            tmp.size.width = tmp.size.height = 0;
            break;
    }

    tmp.max.x = tmp.size.width / 2;
    tmp.max.y = tmp.size.height / 2;
    tmp.min.x = -tmp.max.x;
    tmp.min.y = -tmp.max.y;

    return tmp;

}

int shape_is_solid(shape p, int x, int y) {
    int offset = y * p.size.width + x;
    if (offset < 0 || offset > p.size.width * p.size.height) {
        return 0;
    }
    return (int) p.pixels[offset];
}

int shape_is_solid_o(shape p, int sx, int sy) {
    return shape_is_solid(p, sx - p.min.x, sy - p.min.y);
}
