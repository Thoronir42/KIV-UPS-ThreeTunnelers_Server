#include "net_client.h"

int net_client_init(net_client *p, int socket) {
	p->socket = socket;
	p->status = NET_CLIENT_STATUS_CONNECTED;
	
	return 0;
}
