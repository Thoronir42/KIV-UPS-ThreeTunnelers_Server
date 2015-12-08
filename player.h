#ifndef PLAYER_H
#define	PLAYER_H

#include 'tank.h'


typedef struct player {
	int i;
	long address;
	
	tank* player_tank;
} player;

player* create_player(int id);

void delete_player(player* p);

#endif	/* PLAYER_H */

