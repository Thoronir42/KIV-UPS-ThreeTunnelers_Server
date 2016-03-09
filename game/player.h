#ifndef PLAYER_H
#define	PLAYER_H

#include "tank.h"

const int PLAYER_NONE = 0;
const int PLAYER_SERVER = 1;
const int PLAYER_FIRST_USABLE = 40;

typedef struct player {
	int id;
	long address;
	
	tank* player_tank;
} player;

player* player_create(int id);

void player_delete(player* p);

#endif	/* PLAYER_H */

