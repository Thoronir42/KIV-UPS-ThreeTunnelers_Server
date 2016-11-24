#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "resources.h"
#include "../game/game_room.h"

void _remaclr(void **what, size_t size) {
    *what = malloc(size);
    memset(*what, 0, size);
}

int resources_allocate(resources *p, int rooms, int players_per_room, int connections_reserve) {
    int size;

    printf("Allocating memory for %d*%d=%d players\n", rooms, players_per_room, players_per_room * rooms);

    *(int *) & p->game_rooms_length = rooms;
    *(int *) (&p->clients_length) = rooms * players_per_room;

    _remaclr(&p->clients, sizeof (net_client) * p->clients_length);
    _remaclr(&p->soc_to_client, sizeof (short) * (p->clients_length + connections_reserve));
    _remaclr(&p->game_rooms, sizeof (game_room) * rooms);
    _remaclr(&p->tanks, sizeof (tank) * p->clients_length);

    return 0;
}

int resources_free(resources *p) {
    free(p->clients);
    free(p->soc_to_client);
    free(p->game_rooms);
    free(p->tanks);

    p->clients = NULL;
}

