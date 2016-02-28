#ifndef NETWORKS_H
#define	NETWORKS_H

#define NETWORKS_BUFFER_SIZE 1024

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct networks {
	int server_sock, client_sock, n;
	int server_addr_len, client_addr_len;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	char ch[NETWORKS_BUFFER_SIZE];
} networks;

networks *networks_create();

void networks_delete(networks *p_networks);

#endif	/* NETWORKS_H */

