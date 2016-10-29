#ifndef NETADAPTER_H
#define NETADAPTER_H


#include <netinet/in.h>

#include "net_client.h"

#define NETADAPTER_STATUS_OK 0
#define NETADAPTER_STATUS_SHUTTING_DOWN 1
#define NETADAPTER_STATUS_FINISHED 2
#define NETADAPTER_STATUS_BIND_ERROR -1
#define NETADAPTER_STATUS_LISTEN_ERROR -2
#define NETADAPTER_STATUS_SELECT_ERROR -3

#define NETADAPTER_FD_STD_SKIP 3
#define NETADAPTER_BACKLOG_SIZE 5
#define NETADAPTER_BUFFER_SIZE 512

typedef struct netadapter {
	int status;
	char _buffer[NETADAPTER_BUFFER_SIZE];
	
	struct sockaddr_in addr;
	int port;
	
    int socket;
	
	fd_set client_socks, tests;
	
	net_client *clients;
	int clients_length;
} netadapter;

void *netadapter_thread_select(void *args);

int netadapter_init(netadapter *p, int port);

int netadapter_send_command(int fd, char* c, int length);

#endif /* NETADAPTER_H */

