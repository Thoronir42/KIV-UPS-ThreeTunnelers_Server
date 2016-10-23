#ifndef NET_CLIENT_H
#define NET_CLIENT_H

typedef struct net_client {
	int socket;
	char name[];
	
	clock_t last_active;
	int invalid_counter;
	
} net_client;


#endif /* NET_CLIENT_H */

