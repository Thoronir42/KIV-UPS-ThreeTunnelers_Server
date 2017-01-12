#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../logger.h"
#include "resources.h"
#include "../game/game_room.h"

int resources_allocate(resources *p, int rooms, int players_per_room, int connections) {
    glog(LOG_FINE, "Allocating memory for %d rooms (%d players), %d connections", rooms, players_per_room * rooms, connections);

    *(int *) & p->game_rooms_length = rooms;
    *(int *) (&p->clients_length) = rooms * players_per_room;
    *(int *) (&p->connectons_length) = connections;

    p->clients = calloc(p->clients_length, sizeof (net_client));
    p->connections = calloc(p->connectons_length, sizeof (tcp_connection));
    p->con_to_cli = calloc(p->connectons_length, sizeof (int));
    p->game_rooms = calloc(rooms, sizeof (game_room));

    return 0;
}

void resources_free(resources *p) {
    free(p->clients);
    free(p->connections);
    free(p->con_to_cli);
    free(p->game_rooms);

    p->clients = NULL;
    p->game_rooms = NULL;
    glog(LOG_INFO, "Main: Freeing resources");
}

