#ifndef RESOURCES_H
#define RESOURCES_H

#include "../networks/net_client.h"
#include "../game/player.h"
#include "../model/tank.h"

typedef struct resources {
	net_client *clients;
	const int clients_size;

	player *players;
	const int players_size;

	tank *tanks;
	const int tanks_size;
} resources;

int resources_allocate(resources *p, int rooms, int players_per_room);

int resources_free(resources *p);

#endif /* RESOURCES_H */

