#ifndef RESOURCES_H
#define RESOURCES_H

#include "../networks/net_client.h"
#include "../game/game_room.h"
#include "../game/player.h"
#include "../model/tank.h"

typedef struct resources {
    net_client *clients;
    short *soc_to_client;
    const int clients_length;


    game_room *game_rooms;
    const int game_rooms_length;

    player *players;
    const int players_length;

    tank *tanks;
    const int tanks_length;
} resources;

int resources_allocate(resources *p, int rooms, int players_per_room, int soc_reserve);

int resources_free(resources *p);

#endif /* RESOURCES_H */

