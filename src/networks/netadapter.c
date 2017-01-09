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

#include "../logger.h"
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
                glog(LOG_ERROR, "Unidentified error on initialising adapter");
                break;
            case NETADAPTER_STATUS_BIND_ERROR:
                glog(LOG_ERROR, "Failed to bind to port %d", adapter->port);
                break;
            case NETADAPTER_STATUS_LISTEN_ERROR:
                glog(LOG_ERROR, "Failed to listen at port %d", adapter->port);
                break;
        }
    } else {
        printf("OK\n");
        glog(LOG_INFO, "Port %d is bound and being listened on", adapter->port);
    }
    return adapter->status;
}

void _netadapter_cmd_unhandled(void *handler, network_command cmd) {
    network_command_print("nohandle", &cmd);
}

int netadapter_init(netadapter *p, int port, statistics *stats,
        net_client *clients, int clients_length,
        tcp_connection *connections, int connections_length,
        struct socket_identifier *sock_ids, int sock_ids_length) {
    memset(p, 0, sizeof (netadapter));

    p->port = port;
    p->stats = stats;

    p->clients = clients;
    p->clients_length = clients_length;

    p->sock_ids = sock_ids;
    p->sock_ids_length = sock_ids_length;
    
    p->connections = connections;
    p->connections_length = connections_length;

    p->command_handler = p;
    p->command_handle_func = &_netadapter_cmd_unhandled;

    *(short *) &p->ALLOWED_IDLE_TIME = _NETADAPTER_MAX_IDLE_TIME;
    *(short *) &p->ALLOWED_UNRESPONSIVE_TIME = _NETADAPTER_MAX_IDLE_TIME + 4;
    *(short *) &p->ALLOWED_INVALLID_MSG_COUNT = _NETADAPTER_MAX_WRONG_MSGS;

    return _netadapter_bind_and_listen(p);
}

void netadapter_shutdown(netadapter *p) {
    int ret_val;
    
    FD_CLR(p->socket, &p->client_socks);
    p->status = NETADAPTER_STATUS_SHUTTING_DOWN;
    
    shutdown(p->socket, SHUT_RDWR);
    ret_val = close(p->socket);
    if(!ret_val){
        glog(LOG_INFO, "Netadapter socket closed");
    } else {
        glog(LOG_WARNING, "Netadapter socket could not be closed: %d", ret_val);
    }
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

int netadapter_send_command(netadapter *p, tcp_connection *connection, network_command *cmd) {
    char buffer[sizeof (network_command) + 2];
    int a2write;
    a2write = network_command_to_string(buffer, cmd);

    memcpy(buffer + a2write, "\n\0", 2); // message footer
    write(connection->socket, buffer, a2write + 2);

    p->stats->commands_sent++;
    
    network_command_print("Sent", cmd);

    return 0;
}

int netadapter_broadcast_command(netadapter *p, net_client *clients, int clients_size, network_command *cmd) {
    return netadapter_broadcast_command_p(p, (net_client **)clients, clients_size, cmd, 0);
}

int netadapter_broadcast_command_p(netadapter *p, net_client **clients, int clients_size, network_command *cmd, int ref_pointer) {
    int i, counter = 0;
    net_client *p_cli;
    network_command_print("bc", cmd);
    for (i = 0; i < clients_size; i++) {
        p_cli = ref_pointer ? *(clients + i) : (net_client *)(clients + i);
        if (p_cli->connection.status == TCP_CONNECTION_STATUS_CONNECTED) {
            netadapter_send_command(p, &p_cli->connection, cmd);
            counter++;
        }
    }

    return counter;
}

void _netadapter_connection_kill(netadapter *p, tcp_connection *p_con) {
    close(p_con->socket);
    FD_CLR(p_con->socket, &p->client_socks);
    glog(LOG_INFO, "Connection on socket %d has been terminated", p_con->socket);
}

void netadapter_close_socket_by_client(netadapter *p, net_client *p_cli) {
    _netadapter_connection_kill(p, &p_cli->connection);
    net_client_disconnected(p_cli, 0);
    // todo: inform other clients of disconnection
}

void netadapter_close_socket_by_sid(netadapter *p, socket_identifier *p_sid) {
    tcp_connection *p_con;
    net_client *p_cli;
    int type;
    type = netadapter_unpack_sid(p, p_sid, &p_con, &p_cli);
    
    switch(type){
        case SOCKET_IDENTIFIER_TYPE_CLIENT:
            netadapter_close_socket_by_client(p, p_cli);
            break;
        case SOCKET_IDENTIFIER_TYPE_TBD:
            _netadapter_connection_kill(p, p_con);
            break;
        default:
            glog(LOG_WARNING, "Closing unclosable sock identifier");
            break;
    }

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
    if (offset < 0 || offset >= adapter->clients_length) {
        return -1;
    }

    return offset;
}

void _netadapter_check_idle_client(netadapter *p, net_client *p_client, time_t now) {
    tcp_connection *p_con = &p_client->connection;
    int idle_time = now - p_con->last_active;

    switch (p_client->connection.status) {
        default:
        case TCP_CONNECTION_STATUS_CONNECTED:
            if (idle_time > p->ALLOWED_IDLE_TIME) {
                network_command_strprep(&p_con->_out_buffer,
                        NCT_LEAD_ECHO_REQUEST, loc.netcli_dcreason_unresponsive);
                netadapter_send_command(p, p_con, &p_con->_out_buffer);
                p_client->connection.status = TCP_CONNECTION_STATUS_UNRESPONSIVE;
            }
            break;
        case TCP_CONNECTION_STATUS_UNRESPONSIVE:
            if (idle_time > p->ALLOWED_UNRESPONSIVE_TIME) {
                netadapter_close_socket_by_client(p, p_client);
            }
            break;

        case TCP_CONNECTION_STATUS_DISCONNECTED:
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

    for (i = 0; i < p->clients_length; i++) {
        p_client = p->clients + i;

        if (p_client->connection.status == TCP_CONNECTION_STATUS_EMPTY) {
            continue;
        }
        _netadapter_check_idle_client(p, p_client, now);
    }
}

int netadapter_set_sid(netadapter *p, int socket, int type, int offset) {
    if(socket < 0 || socket > p->sock_ids_length){
        return -1;
    }
    (p->sock_ids + socket)->type = type;
    (p->sock_ids + socket)->offset = offset;
    
    return 0;
}
