#include <stdlib.h>
#include <string.h>

#include "game_room.h"
#include "colors.h"
#include "player.h"
#include "../generic.h"

char game_room_status_letter(unsigned char game_state) {
    switch (game_state) {
        case GAME_ROOM_STATE_DONE:
            return 'D';
        case GAME_ROOM_STATE_IDLE:
            return 'I';
        case GAME_ROOM_STATE_LOBBY:
            return 'L';
        case GAME_ROOM_STATE_STARTNG:
            return 'S';
        case GAME_ROOM_STATE_RUNNING:
            return 'W';
        case GAME_ROOM_STATE_SUMMARIZATION:
            return 'S';
    }
    return '?';
}

int game_room_init(game_room *p, int size, net_client *p_cli) {
    game_room_clean_up(p);

    p->state = GAME_ROOM_STATE_LOBBY;
    p->size = size > GAME_ROOM_MAX_PLAYERS ? GAME_ROOM_MAX_PLAYERS : size;
    colors_init(&p->player_colors);

    int clientRID = game_room_put_client(p, p_cli);
    p->leaderClientRID = clientRID;

    return clientRID;
}

void game_room_clean_up(game_room *p) {
    int i;

    for (i = 0; i < GAME_ROOM_MAX_PLAYERS; i++) {
        p->clients[i] = NULL;
        player_init(p->players + i, ITEM_EMPTY);
    }
    
    p->size = 0;
    p->state = GAME_ROOM_STATE_IDLE;

}

int game_room_count_players(game_room *p) {
    int i, n = 0;

    for (i = 0; i < p->size; i++) {
        if ((p->players + i)->client_rid != ITEM_EMPTY) {
            n++;
        }
    }

    return n;
}

int game_room_get_open_player_slots(game_room *p) {
    return p->size - game_room_count_players(p);
}

int game_room_count_clients(game_room *p) {
    int i, n = 0;

    for (i = 0; i < p->size; i++) {
        if (p->clients[i] != NULL) {
            n++;
        }
    }

    return n;
}

int game_room_get_open_client_slots(game_room *p) {
    return p->size - game_room_count_clients(p);
}

net_client *game_room_get_client(game_room *p, int clientRID) {
    if(clientRID < 0 || clientRID > p->size){
        return NULL;
    }
    
    return p->clients[clientRID];
}
int game_room_find_client(game_room *p, net_client *p_cli) {
    int i;
    for (i = 0; i < p->size; i++) {
        if (p->clients[i] == p_cli) {
            return i;
        }
    }

    return -1;
}

int game_room_put_client(game_room *p_game_room, net_client *p_cli) {
    int i, n = ITEM_EMPTY;

    for (i = 0; i < p_game_room->size; i++) {
        if (p_game_room->clients[i] == p_cli) {
            return i;
        }
        if (p_game_room->clients[i] == NULL && n == ITEM_EMPTY) {
            n = i;
        }
    }
    if (n != ITEM_EMPTY) {
        p_game_room->clients[n] = p_cli;
    }
    return n;
}

void game_room_remove_client(game_room *p, net_client *p_cli) {
    int clientRID = game_room_find_client(p, p_cli);
    if (clientRID == ITEM_EMPTY) {
        return;
    }

    p->clients[clientRID] = NULL;
}

int game_room_is_everyone_ready(game_room *p) {
    int i;
    for (i = 0; i < p->size; i++) {
        if (p->clients[i] != NULL && !p->ready_state[i]) {
            return 0;
        }
    }

    return 1;
}

int game_room_choose_leader_other_than(game_room *p, net_client *p_cli) {
    int i;
    for (i = 0; i < p->size; i++) {
        if (p->clients[i] == p_cli || p->clients[i] == NULL) {
            continue;
        }

        return p->leaderClientRID = i;
    }

    return ITEM_EMPTY;
}

player *game_room_get_player(game_room *p, int playerRID){
    if(playerRID < 0 || playerRID >= p->size){
        return NULL;
    }
    
    return p->players + playerRID;
}

int game_room_attach_player(game_room* p, int clientRID){
    int i;
    for(i =0; i < p->size; i++){
        if(p->players[i].client_rid == ITEM_EMPTY){
            player_init(p->players + i, clientRID);
            return i;
        }
    }
    
    return ITEM_EMPTY;
    
}

void game_room_detach_player(game_room *p, int playerRID) {
    colors_set_in_use(&p->player_colors, p->players[playerRID].color, 0);
    player_init(p->players + playerRID, ITEM_EMPTY);
}
