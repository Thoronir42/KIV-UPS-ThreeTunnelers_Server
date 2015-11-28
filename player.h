#ifndef PLAYER_H
#define	PLAYER_H

typedef struct Player {
	int i;
	long address;
	
	struct tank* player_tank;
} Player;

Player* create_Player(int id);

void delete_Player(Player* p);

#endif	/* PLAYER_H */

