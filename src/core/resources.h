#ifndef RESOURCES_H
#define RESOURCES_H

#include "../networks/net_client.h"
#include "../game/game_room.h"
#include "../game/player.h"
#include "../model/tank.h"
#include "../networks/netadapter.h"

typedef struct resources {
    net_client *clients;
    const int clients_length;
    
    tcp_connection *connections;
    int *con_to_cli;
    const int connectons_length;

    


    game_room *game_rooms;
    const int game_rooms_length;
} resources;

int resources_allocate(resources *p, int rooms, int players_per_room, int connections);

void resources_free(resources *p);

#endif /* RESOURCES_H */

