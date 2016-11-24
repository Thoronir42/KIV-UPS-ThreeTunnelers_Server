#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <time.h>
#include <netinet/in.h>

#include "network_command.h"

#define NET_CLIENT_ID_EMPTY -1

#define NET_CLIENT_STATUS_EMPTY 0
#define NET_CLIENT_STATUS_CONNECTED 1
#define NET_CLIENT_STATUS_DISCONNECTED 2

#define NET_CLIENT_NAME_MAX_LENGTH 24
#define NET_CLIENT_MAX_PLAYERS 2
#define NET_CLIENT_SECRET_LENGTH 6

typedef struct client_connection {
    int socket, a2read;

    struct sockaddr_in addr;
    int addr_len;

    time_t last_active;
    int invalid_counter;

    network_command _out_buffer;
} client_connection;

typedef struct net_client {
    unsigned char status;
    client_connection connection;
    char connection_secret[NET_CLIENT_SECRET_LENGTH];

    char name[NET_CLIENT_NAME_MAX_LENGTH];

    int room_id;
    int player_rids[NET_CLIENT_MAX_PLAYERS];

} net_client;

char client_status_letter(unsigned char status);

int net_client_init(net_client *p, int socket, struct sockaddr_in addr, int addr_len);

void net_client_disconnected(net_client *p, int bool_clean);

int net_client_set_name(net_client *p, const char *name, int length);

#endif /* NET_CLIENT_H */

