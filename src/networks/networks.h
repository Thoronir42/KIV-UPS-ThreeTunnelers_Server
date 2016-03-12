#ifndef NETWORKS_H
#define	NETWORKS_H

#define NETWORKS_BUFFER_SIZE 1024

#define NETWORKS_STATUS_NEW 1
#define NETWORKS_STATUS_RUNNING 3
#define NETWORKS_STATUS_END 4

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#include "network_command.h"
#include "../settings.h"

typedef struct networks {
	settings *p_settings;
	
	int server_sock, client_sock, n;
	unsigned int server_addr_len, client_addr_len;
	struct sockaddr_in *local_addr;
	struct sockaddr_in *remote_addr;
	
	int status;
	char buffer_command[NETWORKS_BUFFER_SIZE];
	
	
} networks;

networks *networks_create();

void networks_delete(networks *p_networks);

void *networks_receiver_run(networks *p_networks);

#endif	/* NETWORKS_H */

