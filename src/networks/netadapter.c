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

    setsockopt(p->socket, SOL_SOCKET, SO_REUSEADDR, &p->socket_reuse, sizeof p->socket_reuse);

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
        tcp_connection *connections, int *con_to_cli, int connections_length) {
    int i;
    memset(p, 0, sizeof (netadapter));

    p->port = port;
    p->socket_reuse = 1;
    p->stats = stats;

    p->clients = clients;
    p->clients_length = clients_length;

    p->connections = connections;
    p->connection_to_client = con_to_cli;
    p->connections_length = connections_length;

    p->command_handler = p;
    p->command_handle_func = &_netadapter_cmd_unhandled;

    *(short *) &p->ALLOWED_IDLE_TIME = _NETADAPTER_MAX_IDLE_TIME;
    *(short *) &p->ALLOWED_UNRESPONSIVE_TIME = _NETADAPTER_MAX_IDLE_TIME + 4;
    *(short *) &p->ALLOWED_INVALLID_MSG_COUNT = _NETADAPTER_MAX_WRONG_MSGS;

    for (i = 0; i < connections_length; i++) {
        p->connection_to_client[i] = (p->connections + i)->socket = NETADAPTER_SOCKET_EMPTY;
    }

    return _netadapter_bind_and_listen(p);
}

void _netadapter_shutdown_connections(netadapter *p) {
    network_command cmd;
    int i, n = 0;

    glog(LOG_INFO, "Netadapter: shutting down all remaining connections");

    network_command_prepare(&cmd, NCT_LEAD_DISCONNECT);
    network_command_set_data(&cmd, g_loc.server_shutting_down, strlen(g_loc.server_shutting_down));

    for (i = 0; i < p->connections_length; i++) {
        if ((p->connections + i)->socket != NETADAPTER_SOCKET_EMPTY) {
            netadapter_send_command(p, p->connections + i, &cmd);
            netadapter_close_connection(p, p->connections + i);
            n++;
        }
    }

    glog(LOG_INFO, "Netadapter: %d connections terminated", n);

}

void netadapter_shutdown(netadapter *p) {
    int ret_val;

    _netadapter_shutdown_connections(p);

    FD_CLR(p->socket, &p->client_socks);
    p->status = NETADAPTER_STATUS_SHUTTING_DOWN;

    shutdown(p->socket, SHUT_RDWR);
    ret_val = close(p->socket);
    if (!ret_val) {
        glog(LOG_INFO, "Netadapter socket closed");
    } else {
        glog(LOG_WARNING, "Netadapter socket could not be closed: %d", ret_val);
    }
}

////  NETADAPTER - command sending

int netadapter_send_command(netadapter *p, tcp_connection *p_con, network_command *cmd) {
    char buffer[sizeof (network_command) + 2];
    int a2write;
    a2write = network_command_to_string(buffer, cmd);

    memcpy(buffer + a2write, "\n\0", 2); // message footer
    a2write += 2;
    write(p_con->socket, buffer, a2write);

    p->stats->commands_sent++;
    p->stats->bytes_sent += a2write;
    //    network_command_print("Sent", cmd);
    
    return 0;
}

int _netadapter_broadcast_command(netadapter *p, net_client **clients, int clients_size, network_command *cmd, int ref_pointer) {
    int i, counter = 0;
    net_client *p_cli;
    for (i = 0; i < clients_size; i++) {
        p_cli = ref_pointer ? *(clients + i) : (net_client *) (clients + i);
        if (p_cli != NULL && p_cli->connection != NULL) {
            netadapter_send_command(p, p_cli->connection, cmd);
            counter++;
        }
    }

    return counter;
}

int netadapter_broadcast_command(netadapter *p, net_client *clients, int clients_size, network_command *cmd) {
    return _netadapter_broadcast_command(p, (net_client **) clients, clients_size, cmd, 0);
}

int netadapter_broadcast_command_p(netadapter *p, net_client **clients, int clients_size, network_command *cmd) {
    return _netadapter_broadcast_command(p, (net_client **) clients, clients_size, cmd, 1);
}

void netadapter_close_connection(netadapter *p, tcp_connection *p_con) {
    net_client *p_cli;
    close(p_con->socket);
    FD_CLR(p_con->socket, &p->client_socks);
    if (p->connection_to_client[p_con->socket] != NETADAPTER_SOCKET_EMPTY) {
        p_cli = (p->clients + p->connection_to_client[p_con->socket]);
        p_cli->connection = NULL;
        p->connection_to_client[p_con->socket] = NETADAPTER_SOCKET_EMPTY;
    }
    glog(LOG_INFO, "Connection on socket %d has been terminated", p_con->socket);
    p_con->socket = NETADAPTER_SOCKET_EMPTY;
}

void netadapter_close_connection_by_client(netadapter *p, net_client *p_cli) {
    netadapter_close_connection(p, p_cli->connection);

}

void netadapter_close_connection_by_socket(netadapter *p, int socket) {
    netadapter_close_connection(p, p->connections + socket);
}

void netadapter_close_connection_msg(netadapter *p, tcp_connection *p_con, const char *msg) {
    network_command_prepare(&p_con->_out_buffer, NCT_LEAD_DISCONNECT);
    network_command_set_data(&p_con->_out_buffer, msg, strlen(msg));

    netadapter_send_command(p, p_con, &p_con->_out_buffer);
    netadapter_close_connection(p, p_con);
}
