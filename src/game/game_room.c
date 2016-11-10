#include <stdlib.h>
#include <string.h>

#include "game_room.h"
#include "player.h"

int game_room_init(game_room *p, int id, player * players, int players_size) {
    int i;

    p->id = id;
    p->game_state = GAME_ROOM_STATE_LOBBY;

    p->players = players;
    p->players_size = players_size;

    for (i = 0; i < players_size; i++) {
        (p->players + i)->id = PLAYER_NONE;
    }

    return 0;
}

int game_room_get_open_slots(game_room *p_game_room) {
    int i, n = 0;

    for (i = 0; i < p_game_room->players_size; i++) {
        if ((p_game_room->players + i)->client_aid == PLAYER_NONE) {
            n++;
        }
    }

    return n;
}