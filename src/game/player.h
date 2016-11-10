#ifndef PLAYER_H
#define PLAYER_H

#include "control_input.h"

// unsigned short
#define PLAYER_NONE 0
#define PLAYER_SERVER 1
#define PLAYER_FIRST_USABLE 12

typedef struct player {
    unsigned short id;
    unsigned short client_aid;
    control_input input;

    int tank_id;
} player;

int player_init(player *p, unsigned short id, unsigned short client_id);

#endif /* PLAYER_H */

