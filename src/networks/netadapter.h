#ifndef NETADAPTER_H
#define NETADAPTER_H

#define NETADAPTER_BUFFER_SIZE 512

typedef struct netadapter {
	char cbuf[NETADAPTER_BUFFER_SIZE];
	
	struct sockaddr_in addr;
    int socket;
	
	fd_set client_socks, tests;
} netadapter;

int netadapter_init(netadapter *p);

int netadapter_start(netadapter *p);

int netadapter_destroy(netadapter *p);

void *netadapter_select_thread(void *args);

#endif /* NETADAPTER_H */

