#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "resources.h"

int resources_allocate(resources *p, int rooms, int players_per_room) {
	printf("Allocating memory for %d*%d=%d players\n", rooms, players_per_room, players_per_room * rooms);
	*(int *) (&p->clients_size) = rooms * players_per_room;
	p->clients = malloc(sizeof (net_client) * p->clients_size);
	memset(p->clients, 0, p->clients_size);

	return 0;
}

int resources_free(resources *p) {
	free(p->clients);
	p->clients = 0;
}

