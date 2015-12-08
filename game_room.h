#ifndef GAMEROOM_H
#define	GAMEROOM_H

#define GR_MAX_PLAYERS 4

typedef struct game_room {
	int id;
	
	
	
} game_room;

*game_room create_game_room(int id);

void delete_game_room(game_room* gr);

#endif	/* GAMEROOM_H */

