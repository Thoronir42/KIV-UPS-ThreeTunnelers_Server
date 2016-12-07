#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "resources.h"
#include "../game/game_room.h"

int resources_allocate(resources *p, int rooms, int players_per_room, int connections_reserve) {
    printf("Allocating memory for %d*%d=%d players\n", rooms, players_per_room, players_per_room * rooms);

    *(int *) & p->game_rooms_length = rooms;
    *(int *) (&p->clients_length) = rooms * players_per_room;
    *(int *) (&p->connectons_length) = connections_reserve;
    *(int *) (&p->sock_ids_length) = p->clients_length + connections_reserve;

    p->clients = calloc(p->clients_length, sizeof (net_client));
    p->connections = calloc(p->connectons_length, sizeof (tcp_connection));
    p->sock_ids = calloc(p->sock_ids_length, sizeof (struct socket_identifier));
    p->game_rooms = calloc(rooms, sizeof (game_room));
    p->tanks = calloc(p->clients_length, sizeof (tank));

    return 0;
}

int resources_free(resources *p) {
    free(p->clients);
    free(p->sock_ids);
    free(p->game_rooms);
    free(p->tanks);

    p->clients = NULL;
}

