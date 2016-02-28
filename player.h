#ifndef PLAYER_H
#define	PLAYER_H

#include "tank.h"


typedef struct player {
	int id;
	long address;
	
	tank* player_tank;
} player;

player* player_create(int id);

void player_delete(player* p);

#endif	/* PLAYER_H */

