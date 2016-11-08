#include <string.h>

#include "net_client.h"

int net_client_init(net_client *p, int socket, struct sockaddr_in addr, int addr_len) {
    memset(p, 0, sizeof (net_client));
    p->socket = socket;
    p->status = NET_CLIENT_STATUS_CONNECTED;
    p->addr = addr;
    p->addr_len = addr_len;

    return 0;
}

void net_client_disconnected(net_client *p) {
    p->status = NET_CLIENT_STATUS_DISCONNECTED;
}

void net_client_cleanup(net_client *p) {
    p->status = NET_CLIENT_STATUS_EMPTY;
}

