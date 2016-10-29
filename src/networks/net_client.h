#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <time.h>
#include <netinet/in.h>

#define NET_CLIENT_NAME_MAX_LENGTH 16

typedef struct net_client {
	int socket;
	int a2read;
	
	struct sockaddr_in addr;
	int addr_len;
	char name[NET_CLIENT_NAME_MAX_LENGTH];
	
	clock_t last_active;
	int invalid_counter;
	
} net_client;


#endif /* NET_CLIENT_H */

