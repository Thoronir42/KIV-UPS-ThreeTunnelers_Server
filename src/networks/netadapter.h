#ifndef NETADAPTER_H
#define NETADAPTER_H


#include <netinet/in.h>

#define NETADAPTER_STATUS_FINISHED 0
#define NETADAPTER_STATUS_OK 1
#define NETADAPTER_STATUS_BIND_ERROR -1
#define NETADAPTER_STATUS_LISTEN_ERROR -2
#define NETADAPTER_STATUS_SELECT_ERROR -3

#define NETADAPTER_BUFFER_SIZE 512

typedef struct netadapter {
	int status;
	char cbuf[NETADAPTER_BUFFER_SIZE];
	
	struct sockaddr_in addr;
	int port;
	
    int socket;
	
	fd_set client_socks, tests;
} netadapter;

void *netadapter_select_thread(void *args);

int netadapter_init(netadapter *p, int port);

#endif /* NETADAPTER_H */

