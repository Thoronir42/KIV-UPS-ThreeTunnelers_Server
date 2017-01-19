#ifndef _GAME_ROOM_H
#define _GAME_ROOM_H

#include "player.h"
#include "colors.h"
#include "warzone.h"

#include "../map/tunneler_map.h"

#include "../networks/net_client.h"
#include "../model/tank.h"
#include "../model/projectile.h"

#define GAME_ROOM_MAX_PLAYERS WARZONE_MAX_PLAYERS

#define GAME_ROOM_STATE_IDLE 0
#define GAME_ROOM_STATE_LOBBY 1
#define GAME_ROOM_STATE_STARTNG 2
#define GAME_ROOM_STATE_RUNNING 3
#define GAME_ROOM_STATE_SUMMARIZATION 4
#define GAME_ROOM_STATE_DONE 5

typedef struct game_room
{
    unsigned char state;

    int size;

    int leaderClientRID;
    net_client *clients[GAME_ROOM_MAX_PLAYERS];
    char ready_state[GAME_ROOM_MAX_PLAYERS];

    colors player_colors;
    player players[GAME_ROOM_MAX_PLAYERS];
    
    warzone zone;


} game_room;

char game_room_status_letter(unsigned char game_state);

/**
 * Prepares game room and puts given client into it. Returns clients RID.
 * @param p
 * @param size
 * @param p_cli
 * @return 
 */
int game_room_init(game_room *p, int size, net_client *p_cli);

void game_room_clean_up(game_room *p);

int game_room_get_open_player_slots(game_room *p_game_room);
int game_room_get_open_client_slots(game_room *p_game_room);


int game_room_find_client(game_room *p, net_client *p_cli);
/**
 * Tries to place client into room - if client already is in room, returns his
 * current RID. Otherwise looks for first empty client slot, if one exists,
 * puts client there and returns its RID. Otherwise returns -1
 * @param p_game_room
 * @param p_cli
 * @return 
 */
int game_room_put_client(game_room *p_gr, net_client *p_cli);

void game_room_remove_client(game_room *p, net_client *p_cli);

int game_room_is_everyone_ready(game_room *p);

void game_room_attach_player(game_room *p, int clientRID);

void game_room_detach_player(game_room *p, int playerRID);

#endif /* _GAME_ROOM_H */

