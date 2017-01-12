#ifndef RESOURCES_H
#define RESOURCES_H

#include "../networks/net_client.h"
#include "../game/game_room.h"
#include "../game/player.h"
#include "../model/tank.h"
#include "../networks/netadapter.h"

#define RESOURCES_ERROR_ALLOCATION_FAILED 1
#define RESOURCES_ERROR_NOT_ENOUGH_CONNECTIONS_AVAILABLE 2


#define RESOURCES_SPARE_CONNECTIONS 20

typedef struct resources {
    net_client *clients;
    const int clients_length;
    
    tcp_connection *connections;
    int *con_to_cli;
    const int connections_length;

    game_room *game_rooms;
    const int game_rooms_length;
} resources;

int resources_allocate(resources *p, int rooms, int players_per_room, int connections);

void resources_free(resources *p);

#endif /* RESOURCES_H */

