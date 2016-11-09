#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <time.h>
#include <netinet/in.h>

#define NET_CLIENT_ID_EMPTY -1

#define NET_CLIENT_STATUS_EMPTY 0
#define NET_CLIENT_STATUS_CONNECTED 1
#define NET_CLIENT_STATUS_DISCONNECTED 2

#define NET_CLIENT_NAME_MAX_LENGTH 24
#define NET_CLIENT_MAX_PLAYERS 2
#define NET_CLIENT_SECRET_LENGTH 6

typedef struct net_client {
	unsigned char status;

	int socket, a2read;
	char connection_secret[NET_CLIENT_SECRET_LENGTH];

	struct sockaddr_in addr;
	int addr_len;
	char name[NET_CLIENT_NAME_MAX_LENGTH];

	clock_t last_active;
	int invalid_counter;

	int room_id;
	int player_rids[NET_CLIENT_MAX_PLAYERS];

} net_client;

char net_client_get_status_letter(unsigned char status);

int net_client_init(net_client *p, int socket, struct sockaddr_in addr, int addr_len);

void net_client_disconnected(net_client *p);

void net_client_cleanup(net_client *p);

#endif /* NET_CLIENT_H */

