#ifndef _GAME_ROOM_H
#define _GAME_ROOM_H

#define GAME_ROOM_STATE_IDLE 0
#define GAME_ROOM_STATE_LOBBY 1
#define GAME_ROOM_STATE_RUNNING 2
#define GAME_ROOM_STATE_DONE 4

#include "player.h"
#include "../map/tunneler_map.h"

const int GAME_ROOM_MAX_PLAYERS;


typedef struct game_room {
    int id;
    char game_state;

    int max_players;
    player *players;


} game_room;

game_room *game_room_create(int id, int max_players);

void game_room_delete(game_room *p_game_room);

int game_room_get_open_slots(game_room *p_game_room);

#endif /* _GAME_ROOM_H */

