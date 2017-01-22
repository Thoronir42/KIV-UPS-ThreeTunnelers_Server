#ifndef PLAYER_H
#define PLAYER_H

#include "control_input.h"

// unsigned short
#define PLAYER_NONE 0
#define PLAYER_SERVER 1
#define PLAYER_FIRST_USABLE 12

typedef struct player {
    int client_rid;

    control_input input;
    int color;
} player;

int player_init(player *p, int client_rid, int color);

void player_cleanup(player *p);

#endif /* PLAYER_H */

