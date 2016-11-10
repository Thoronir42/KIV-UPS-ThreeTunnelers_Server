#ifndef NETADAPTER_H
#define NETADAPTER_H


#include <netinet/in.h>

#include "net_client.h"
#include "network_command.h"

#define NETADAPTER_STATUS_OK 0
#define NETADAPTER_STATUS_SHUTTING_DOWN 1
#define NETADAPTER_STATUS_FINISHED 2
#define NETADAPTER_STATUS_BIND_ERROR -1
#define NETADAPTER_STATUS_LISTEN_ERROR -2
#define NETADAPTER_STATUS_SELECT_ERROR -3

#define NETADAPTER_FD_STD_SKIP 3
#define NETADAPTER_FD_RESERVE 20

#define NETADAPTER_BACKLOG_SIZE 5
#define NETADAPTER_BUFFER_SIZE 512

#define _NETADAPTER_MAX_WRONG_MSGS 3
#define _NETADAPTER_MAX_IDLE_TIME 5

typedef struct netadapter {
    int status;
    char _buffer[NETADAPTER_BUFFER_SIZE];

    struct sockaddr_in addr;
    int port;

    int socket;

    fd_set client_socks;


    net_client *clients;
    int clients_size;
    short *fd_to_client;

    const short ALLOWED_IDLE_TIME;
    const short ALLOWED_INVALLID_MSG_COUNT;
} netadapter;


int netadapter_init(netadapter *p, int port, net_client *clients, int clients_size, short *fd_to_client);
void netadapter_shutdown(netadapter *p);

void *netadapter_thread_select(void *args);

int netadapter_send_command(net_client *client, network_command * cmd);
int netadapter_broadcast_command(net_client *clients, int clients_size, network_command *cmd);

int netadapter_client_aid_by_client(netadapter *adapter, net_client *p_cl);

net_client *netadapter_get_client_by_fd(netadapter *p, int fd);

void netadapter_check_idle_clients(netadapter *p);

#endif /* NETADAPTER_H */

