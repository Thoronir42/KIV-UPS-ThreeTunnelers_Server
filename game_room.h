#ifndef GAMEROOM_H
#define	GAMEROOM_H

#include "player.h"

#define GAME_ROOM_MAX_PLAYERS 4
#define PASSWORD_MAX_LENGTH 32

#define GAME_ROOM_STATE_LOBBY 1
#define GAME_ROOM_STATE_RUNNING 2
#define GAME_ROOM_STATE_DONE 3

typedef struct game_room {
	int id;
	char password[PASSWORD_MAX_LENGTH];
	byte game_state;
	
	int max_players;
	player players[];
	
	
} game_room;

game_room *game_room_create(int id, int max_players);

void game_room_delete(game_room *p_game_room);

int game_room_has_password(game_room *p_game_room);

int game_room_get_open_slots(game_room *p_game_room);

#endif	/* GAMEROOM_H */

