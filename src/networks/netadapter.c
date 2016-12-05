#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include "net_client.h"
#include "netadapter.h"

#include "../my_strings.h"
#include "network_command.h"
#include "../core/engine.h"
#include "../localisation.h"

////  NETADAPTER - initialisation

int _netadapter_init_socket(netadapter *p) {
    memset(&p->addr, 0, sizeof (struct sockaddr_in));

    p->socket = socket(AF_INET, SOCK_STREAM, 0);

    p->addr.sin_family = AF_INET;
    p->addr.sin_port = htons(p->port);
    p->addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(p->socket, (struct sockaddr *) &p->addr, \
		sizeof (struct sockaddr_in))) {
        return NETADAPTER_STATUS_BIND_ERROR;
    }
    if (listen(p->socket, NETADAPTER_BACKLOG_SIZE)) {
        return NETADAPTER_STATUS_LISTEN_ERROR;
    }

    return NETADAPTER_STATUS_OK;
}

int _netadapter_bind_and_listen(netadapter *adapter) {
    printf("Netadapter: Bind and listen on port %d...", adapter->port);
    adapter->status = _netadapter_init_socket(adapter);
    if (adapter->status != NETADAPTER_STATUS_OK) {
        printf("ER\n");
        switch (adapter->status) {
            default:
                printf("Unidentified error on initialising adapter\n");
                break;
            case NETADAPTER_STATUS_BIND_ERROR:
                printf("Failed to bind to port %d\n", adapter->port);
                break;
            case NETADAPTER_STATUS_LISTEN_ERROR:
                printf("Failed to listen at port %d\n", adapter->port);
                break;
        }
    } else {
        printf("OK\n");
    }
    return adapter->status;
}

void _netadapter_cmd_unhandled(void *handler, network_command cmd) {
    network_command_print("nohandle", &cmd);
}

int netadapter_init(netadapter *p, int port,
        net_client *clients, int clients_size,
        struct socket_identifier *sock_ids, int sock_ids_length) {
    memset(p, 0, sizeof (netadapter));

    p->port = port;

    p->clients = clients;
    p->clients_size = clients_size;

    p->sock_ids = sock_ids;
    p->sock_ids_length = sock_ids_length;

    p->command_handler = p;
    p->command_handle_func = &_netadapter_cmd_unhandled;

    *(short *) &p->ALLOWED_IDLE_TIME = _NETADAPTER_MAX_IDLE_TIME;
    *(short *) &p->ALLOWED_UNRESPONSIVE_TIME = _NETADAPTER_MAX_IDLE_TIME + 4;
    *(short *) &p->ALLOWED_INVALLID_MSG_COUNT = _NETADAPTER_MAX_WRONG_MSGS;

    return _netadapter_bind_and_listen(p);
}

void netadapter_shutdown(netadapter *p) {
    p->status = NETADAPTER_STATUS_SHUTTING_DOWN;
}

int netadapter_unpack_sid(netadapter *p, socket_identifier *sid,
        tcp_connection **p_con, net_client **p_cli) {
    switch (sid->type) {
        case SOCKET_IDENTIFIER_TYPE_TBD:
            *p_con = p->connections + sid->offset;
            break;
        case SOCKET_IDENTIFIER_TYPE_CLIENT:
            *p_cli = p->clients + sid->offset;
            *p_con = &(*p_cli)->connection;
            break;
        default:
            return SOCKET_IDENTIFIER_TYPE_EMPTY;
    }
    return sid->type;
}

////  NETADAPTER - command sending

int netadapter_send_command(tcp_connection *connection, network_command *cmd) {
    char buffer[sizeof (network_command) + 2];
    int a2write;
    a2write = network_command_to_string(buffer, cmd);

    memcpy(buffer + a2write, "\n\0", 2); // message footer
    write(connection->socket, buffer, a2write + 2);

    network_command_print("Sent", cmd);

    return 0;
}

int netadapter_broadcast_command(net_client *clients, int clients_size, network_command *cmd) {
    int i, counter = 0;
    network_command_print("bc", cmd);
    for (i = 0; i < clients_size; i++) {
        if ((clients + i)->status == NET_CLIENT_STATUS_CONNECTED) {
            netadapter_send_command(&(clients + i)->connection, cmd);
            counter++;
        }
    }

    return counter;
}

void _netadapter_connection_kill(netadapter *p, tcp_connection *p_con) {
    close(p_con->socket);
    FD_CLR(p_con->socket, &p->client_socks);
}

void netadapter_close_socket_by_client(netadapter *p, net_client *p_cli) {
    _netadapter_connection_kill(p, &p_cli->connection);
    net_client_disconnected(p_cli, 0);
}

void netadapter_close_socket_by_sid(netadapter *p, socket_identifier *p_sid) {
    tcp_connection *p_con;
    net_client *p_cli;
    netadapter_unpack_sid(p, p_sid, &p_con, &p_cli);
    
}

//// NETADAPTER - client controls

net_client *netadapter_get_client_by_aid(netadapter *p, int aid) {
    return (p->clients + aid);
}

socket_identifier *netadapter_get_sid_by_socket(netadapter *p, int socket) {
    // TODO: verify less part of unequation
    if (socket < 0 || socket >= p->sock_ids_length) {
        return NULL;
    }
    return p->sock_ids + socket;
}

int netadapter_client_aid_by_client(netadapter *adapter, net_client *p_cl) {
    int offset = p_cl - adapter->clients;
    if (offset < 0 || offset >= adapter->clients_size) {
        return -1;
    }

    return offset;
}

void _netadapter_check_idle_client(netadapter *p, net_client *p_client, time_t now) {
    tcp_connection *p_con = &p_client->connection;
    int idle_time = now - p_con->last_active;

    switch (p_client->status) {
        default:
        case NET_CLIENT_STATUS_CONNECTED:
            if (idle_time > p->ALLOWED_IDLE_TIME) {
                network_command_strprep(&p_con->_out_buffer,
                        NCT_LEAD_STILL_THERE, loc.netcli_dcreason_unresponsive);
                netadapter_send_command(p_con, &p_con->_out_buffer);
                p_client->status = NET_CLIENT_STATUS_UNRESPONSIVE;
            }
            break;
        case NET_CLIENT_STATUS_UNRESPONSIVE:
            if (idle_time > p->ALLOWED_UNRESPONSIVE_TIME) {
                netadapter_close_socket_by_client(p, p_client);
            }
            break;

        case NET_CLIENT_STATUS_DISCONNECTED:
            if (idle_time > p->ALLOWED_IDLE_TIME) {
                net_client_disconnected(p_client, 1);
            }
            break;
    }
}

void netadapter_check_idle_clients(netadapter *p) {
    int i;
    net_client *p_client;
    time_t now = time(NULL);

    for (i = 0; i < p->clients_size; i++) {
        p_client = p->clients + i;

        if (p_client->status == NET_CLIENT_STATUS_EMPTY) {
            continue;
        }
        _netadapter_check_idle_client(p, p_client, now);
    }
}
