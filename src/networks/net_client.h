#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <time.h>
#include <netinet/in.h>

#include "network_command.h"

#define TCP_CONNECTION_BUFFER_SIZE (NETWORK_COMMAND_DATA_LENGTH + NETWORK_COMMAND_HEADER_SIZE)

#define NET_CLIENT_ID_EMPTY -1

#define NET_CLIENT_STATUS_ANY -1
#define NET_CLIENT_STATUS_EMPTY 0
#define NET_CLIENT_STATUS_CONNECTED 1
#define NET_CLIENT_STATUS_UNRESPONSIVE 2
#define NET_CLIENT_STATUS_DISCONNECTED 3

#define NET_CLIENT_NAME_MAX_LENGTH 24
#define NET_CLIENT_MAX_PLAYERS 2
#define NET_CLIENT_SECRET_LENGTH 6

typedef struct tcp_connection {
    int socket, a2read;

    struct sockaddr_in addr;
    int addr_len;

    time_t last_active;
    int invalid_counter;
    
    int client_aid;
    
    char _in_buffer[NETWORK_COMMAND_DATA_LENGTH + NETWORK_COMMAND_HEADER_SIZE];
    int _in_buffer_ptr;
    network_command _out_buffer;
    
} tcp_connection;

typedef struct net_client {
    tcp_connection *connection;
    unsigned char status;
    char connection_secret[NET_CLIENT_SECRET_LENGTH + 1];

    char name[NET_CLIENT_NAME_MAX_LENGTH];

    int room_id;
    int player_rids[NET_CLIENT_MAX_PLAYERS];

} net_client;

char net_client_status_letter(unsigned char status);
int  tcp_connection_process(tcp_connection *p, int a2read);

int  net_client_init(net_client *p, tcp_connection *connection);
void net_client_disconnected(net_client *p, int bool_clean);
int  net_client_set_name(net_client *p, const char *name, int length);

#endif /* NET_CLIENT_H */

