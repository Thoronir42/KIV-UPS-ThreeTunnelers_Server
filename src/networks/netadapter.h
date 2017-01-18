#ifndef NETADAPTER_H
#define NETADAPTER_H


#include <netinet/in.h>

#include "../statistics.h"
#include "net_client.h"
#include "network_command.h"

#define NETADAPTER_STATUS_OK 0
#define NETADAPTER_STATUS_SHUTTING_DOWN 1
#define NETADAPTER_STATUS_FINISHED 2
#define NETADAPTER_STATUS_BIND_ERROR -1
#define NETADAPTER_STATUS_LISTEN_ERROR -2
#define NETADAPTER_STATUS_SELECT_ERROR -3

#define NETADAPTER_FD_STD_SKIP 3

#define NETADAPTER_BACKLOG_SIZE 5
#define NETADAPTER_BUFFER_SIZE 512

#define NETADAPTER_ITEM_EMPTY -1

#define NETADAPTER_SOCK_ERROR_NOTHING_TO_READ -1
#define NETADAPTER_SOCK_ERROR_INVALID_MSG_COUNT_EXCEEDED -2
#define NETADAPTER_SOCK_ERROR_MSG_TOO_LONG -3
#define NETADAPTER_SOCK_ERROR_MSG_TOO_SHORT -4
#define NETADAPTER_SOCK_ERROR_AUTHORIZATION_FAIL -5


#define _NETADAPTER_MAX_WRONG_MSGS 3
#define _NETADAPTER_MAX_IDLE_TIME 5

typedef struct netadapter
{
    int status;

    struct sockaddr_in addr;
    int port;
    
    int socket_reuse;
    int socket;

    fd_set client_socks;

    network_command _cmd_in_buffer;

    net_client *clients;
    int clients_length;
    
    tcp_connection *connections;
    int *connection_to_client;
    int connections_length;
    
    const short ALLOWED_IDLE_TIME;
    const short ALLOWED_UNRESPONSIVE_TIME;
    const short ALLOWED_INVALLID_MSG_COUNT;

    void *command_handler;
    void (*command_handle_func)(void *handler, network_command cmd);
    
    statistics *stats;
} netadapter;


int netadapter_init(netadapter *p, int port, statistics *stats,
        net_client *clients, int clients_length,
        tcp_connection *connections, int *con_to_cli, int connections_length);

void netadapter_shutdown(netadapter *p);

//// thread select
void *netadapter_thread_select(void *args);

//// netadapter controls
void netadapter_handle_invallid_command(netadapter *p, net_client *p_cli, network_command cmd);

int netadapter_send_command(netadapter *p, tcp_connection *connection, network_command *cmd);
int netadapter_broadcast_command(netadapter *p, net_client *clients, int clients_size, network_command *cmd);
int netadapter_broadcast_command_p(netadapter *p, net_client **clients, int clients_size, network_command *cmd);

void netadapter_close_connection(netadapter *p, tcp_connection *p_con);
void netadapter_close_connection_by_client(netadapter *p, net_client *p_cli);
void netadapter_close_connection_by_socket(netadapter *p, int socket);

//// netadapter accessors
int netadapter_client_aid_by_client(netadapter *adapter, net_client *p_cl);
int netadapter_client_aid_by_socket(netadapter *p, int socket);
net_client *netadapter_get_client_by_aid(netadapter *p, int aid);


#endif /* NETADAPTER_H */

