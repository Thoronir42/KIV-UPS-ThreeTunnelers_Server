#ifndef _GAME_ROOM_H
#define _GAME_ROOM_H

#define GAME_ROOM_MAX_PLAYERS 4
#define GAME_ROOM_MAX_PROJECTILES GAME_ROOM_MAX_PLAYERS * 20

#define GAME_ROOM_STATE_IDLE 0
#define GAME_ROOM_STATE_LOBBY 1
#define GAME_ROOM_STATE_RUNNING 2
#define GAME_ROOM_STATE_SUMMARIZATION 3
#define GAME_ROOM_STATE_DONE 4

#include "player.h"
#include "colors.h"
#include "../map/tunneler_map.h"

#include "../networks/net_client.h"
#include "../model/tank.h"
#include "../model/projectile.h"

typedef struct game_room {
    char game_state;

    int size;

    int leaderClient;
    net_client *clients[GAME_ROOM_MAX_PLAYERS];
    
    colors player_colors;
    player players[GAME_ROOM_MAX_PLAYERS];
    tank tanks[GAME_ROOM_MAX_PLAYERS];
    projectile projectiles[GAME_ROOM_MAX_PROJECTILES];
    
    
} game_room;

int game_room_init(game_room *p, int size);

void game_room_clean_up(game_room *p);


int game_room_get_open_player_slots(game_room *p_game_room);
int game_room_get_open_client_slots(game_room *p_game_room);
int game_room_put_client(game_room *p_gr, net_client *p_cli);

#endif /* _GAME_ROOM_H */

