#ifndef GAMEROOM_H
#define	GAMEROOM_H

#define GAME_ROOM_MAX_PLAYERS 4
#define PASSWORD_MAX_LENGTH 32

typedef struct game_room {
	int id;
	char password[PASSWORD_MAX_LENGTH];
	
	
	
} game_room;

game_room *game_room_create(int id);

void game_room_delete(game_room *p_game_room);

#endif	/* GAMEROOM_H */

