#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../logger.h"
#include "resources.h"
#include "../game/game_room.h"

int resources_allocate(resources *p, int rooms, int players_per_room, int max_connections) {
    *(int *) & p->game_rooms_length = rooms;
    *(int *) (&p->clients_length) = rooms * players_per_room;
    
    if(p->clients_length  + RESOURCES_SPARE_CONNECTIONS > max_connections){
        return RESOURCES_ERROR_NOT_ENOUGH_CONNECTIONS_AVAILABLE;
    }
    
    *(int *) (&p->connections_length) = p->clients_length + RESOURCES_SPARE_CONNECTIONS;
    

    glog(LOG_FINE, "Allocating memory for %d rooms (%d players + %d clients), %d connections", 
            p->game_rooms_length, p->clients_length, p->clients_length, p->connections_length);
    
    p->clients = calloc(p->clients_length, sizeof (net_client));
    p->connections = calloc(p->connections_length, sizeof (tcp_connection));
    p->con_to_cli = calloc(p->connections_length, sizeof (int));
    p->game_rooms = calloc(rooms, sizeof (game_room));
    
    if(p->clients == NULL || p->connections == NULL || p->con_to_cli == NULL || p->game_rooms == NULL){
        return RESOURCES_ERROR_ALLOCATION_FAILED;
    }
    
    glog(LOG_FINE, "Allocation successfull");
    
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

