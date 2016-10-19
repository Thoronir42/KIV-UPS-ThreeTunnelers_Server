#ifndef GAMEROOM_H
#define	GAMEROOM_H

#include "player.h"

const int GAME_ROOM_MAX_PLAYERS;

const int GAME_ROOM_STATE_LOBBY,
	GAME_ROOM_STATE_RUNNING,
	GAME_ROOM_STATE_DONE;

typedef struct game_room {
	int id;
	char game_state;
	
	
	
	int max_players;
	player *players;
	
	
} game_room;

game_room *game_room_create(int id, int max_players);

void game_room_delete(game_room *p_game_room);

int game_room_get_open_slots(game_room *p_game_room);

#endif	/* GAMEROOM_H */

