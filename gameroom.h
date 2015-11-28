#ifndef GAMEROOM_H
#define	GAMEROOM_H

typedef struct gameroom {
	int id;
	int maxPlayers;
	
	
} GameRoom;

*GameRoom create_GameRoom(int id);

void delete_GameRoom(GameRoom* gr);

#endif	/* GAMEROOM_H */

