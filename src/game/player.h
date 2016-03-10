#ifndef PLAYER_H
#define	PLAYER_H

#include "tank.h"

const int PLAYER_NONE;
const int PLAYER_SERVER;
const int PLAYER_FIRST_USABLE;

typedef struct player {
	int id;
	long address;
	
	tank* player_tank;
} player;

player* player_create(int id);

void player_delete(player* p);

#endif	/* PLAYER_H */

