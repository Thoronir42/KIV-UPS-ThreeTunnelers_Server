#include <string.h>

#include "net_client.h"

char net_client_status_letter(unsigned char status) {
    switch (status) {
        default:
            return '?';
        case NET_CLIENT_STATUS_CONNECTED:
            return 'C';
        case NET_CLIENT_STATUS_UNRESPONSIVE:
            return 'U';
        case NET_CLIENT_STATUS_EMPTY:
            return 'E';
    }
}

int net_client_init(net_client *p, tcp_connection *connection) {
    memset(p, 0, sizeof (net_client));
    p->connection = connection;

    return 0;
}

void net_client_disconnected(net_client *p, int bool_clean) {
    p->connection = NULL;
}

int net_client_set_name(net_client *p, const char *name, int length) {
    int diff = 0;

    if (length > NET_CLIENT_NAME_MAX_LENGTH) {
        diff = length - NET_CLIENT_NAME_MAX_LENGTH;
        length = NET_CLIENT_NAME_MAX_LENGTH;
    }

    memcpy(p->name, name, length);

    return diff;
}

void tcp_connection_reset(tcp_connection *p) {
    memset(p, 0, sizeof (tcp_connection));
}
