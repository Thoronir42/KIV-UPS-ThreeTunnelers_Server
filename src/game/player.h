#ifndef PLAYER_H
#define PLAYER_H

#include "control_input.h"

// unsigned short
#define PLAYER_NONE 0
#define PLAYER_SERVER 1
#define PLAYER_FIRST_USABLE 12

typedef struct player {
    unsigned short client_rid;

    control_input input;
    int color;
} player;

int player_init(player *p, unsigned short client_rid);

#endif /* PLAYER_H */

