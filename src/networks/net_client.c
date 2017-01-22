#include <string.h>
#include <stdio.h>

#include "net_client.h"
#include "../generic.h"

char net_client_status_letter(net_client_status status) {
    switch (status) {
        default:
            return '?';
        case NET_CLIENT_STATUS_CONNECTED:
            return 'C';
        case NET_CLIENT_STATUS_UNRESPONSIVE:
            return 'U';
        case NET_CLIENT_STATUS_DISCONNECTED:
            return 'D';
        case NET_CLIENT_STATUS_EMPTY:
            return 'E';
    }
}

int net_client_init(net_client *p, tcp_connection *connection) {
    net_client_wipe(p);
    p->connection = connection;

    return 0;
}

void net_client_wipe(net_client *p) {
    int i;
    memset(p, 0, sizeof(net_client));
    for(i = 0; i < NET_CLIENT_MAX_PLAYERS; i++){
        p->player_rids[i] = ITEM_EMPTY;
    }
    p->room_id = ITEM_EMPTY;
}

int net_client_set_name(net_client *p, const char *name, int length) {
    int diff = 0;

    if (length > NET_CLIENT_NAME_MAX_LENGTH) {
        diff = length - NET_CLIENT_NAME_MAX_LENGTH;
        length = NET_CLIENT_NAME_MAX_LENGTH;
    }

    memcpy(p->name, name, length);

    return diff;
}

int net_client_put_player(net_client *p, int playerRID){
    int i = 0;
    for(i = 0; i < NET_CLIENT_MAX_PLAYERS; i++){
        if(p->player_rids[i] == ITEM_EMPTY){
            p->player_rids[i] = playerRID;
            return i;
        }
    }
    
    return ITEM_EMPTY;
}
