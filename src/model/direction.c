#include "direction.h"

enum direction _g_directions_by_d[] = {
    DIRECTION_NW, DIRECTION_N, DIRECTION_NE,
    DIRECTION_W, DIRECTION_0, DIRECTION_E,
    DIRECTION_SW, DIRECTION_S, DIRECTION_SE
};

void directions_initialise() {
    // -1 on X (West)
    G_DIRECTIONS[DIRECTION_NW].x = G_DIRECTIONS[DIRECTION_W].x = G_DIRECTIONS[DIRECTION_SW].x = -1;
    //  0 on X (neutral)
    G_DIRECTIONS[DIRECTION_N].x = G_DIRECTIONS[DIRECTION_0].x = G_DIRECTIONS[DIRECTION_S].x = 0;
    // +1 on X (East)
    G_DIRECTIONS[DIRECTION_NE].x = G_DIRECTIONS[DIRECTION_E].x = G_DIRECTIONS[DIRECTION_SE].x = +1;

    // -1 on Y (North)
    G_DIRECTIONS[DIRECTION_NW].y = G_DIRECTIONS[DIRECTION_N].y = G_DIRECTIONS[DIRECTION_NE].y = -1;
    //  0 on Y (neutral)
    G_DIRECTIONS[DIRECTION_W].y = G_DIRECTIONS[DIRECTION_0].y = G_DIRECTIONS[DIRECTION_E].y = 0;
    // +1 on Y (South)
    G_DIRECTIONS[DIRECTION_SW].y = G_DIRECTIONS[DIRECTION_S].y = G_DIRECTIONS[DIRECTION_SE].y = +1;
}

int sign(int val) {
    return val < 0 ? -1 : (val > 0 ? 1 : 0);
}

enum direction direction_get_by_d(int dx, int dy) {
    int x = sign(dx) + 1;
    int y = sign(dy) + 1;
    return _g_directions_by_d[y * 3 + x];
}
