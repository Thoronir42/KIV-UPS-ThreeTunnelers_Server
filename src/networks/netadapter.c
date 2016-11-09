#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include <string.h>

#include "net_client.h"
#include "netadapter.h"

#include "../my_strings.h"
#include "network_command.h"
#include "../core/engine.h"

////  THREAD_SELECT - PROCESS INCOMMING STUFF

int _netadapter_process_message(int socket, char *buffer, int read_size, network_command *command) {
    int length;

    length = NETADAPTER_BUFFER_SIZE - 2;
    if (read_size < length) {
        length = read_size;
    }

    read(socket, buffer, length);
    return network_command_from_string(command, buffer);

}

void _netadapter_close_client(netadapter *adapter, net_client *p_cl) {
    close(p_cl->socket);
    FD_CLR(p_cl->socket, &adapter->client_socks);
    net_client_disconnected(p_cl);
}

void _netadapter_handle_invalid_message(netadapter *adapter, net_client *p_cl) {
    network_command cmd;
    memset(&cmd, 0, sizeof (network_command));

    cmd.type = NET_CMD_BAD_FORMAT;
    read(p_cl->socket, cmd.data, p_cl->a2read);
    netadapter_send_command(p_cl, &cmd);

    if (++(p_cl->invalid_counter) > NETADAPTER_PATIENCE) {
        printf("Netadapter: client on socket %02d kept sending gibberish. They "
                "will be terminated immediately.\n", p_cl->socket);
        _netadapter_close_client(adapter, p_cl);
    }
}

void _netadapter_ts_process_client_socket(netadapter *adapter, net_client *p_cl) {
    network_command cmd_in;
    memset(adapter->_buffer, 0, NETADAPTER_BUFFER_SIZE);

    ioctl(p_cl->socket, FIONREAD, &p_cl->a2read);
    if (p_cl->a2read < NETWORK_COMMAND_HEADER_SIZE) { // mame co cist
        if (p_cl->a2read <= 0) {
            printf("Netadapter: something wrong happened with client on socket "
                    "%02d. They will be put down now.\n", p_cl->socket);
            _netadapter_close_client(adapter, p_cl);
            return;
        } else {
            _netadapter_handle_invalid_message(adapter, p_cl);
        }

    } else {
        _netadapter_process_message(p_cl->socket, adapter->_buffer, p_cl->a2read, &cmd_in);
        cmd_in.client_aid = netadapter_client_aid_by_client(adapter, p_cl); // fixme: client_aid

        network_command_print("Received", &cmd_in);
        engine_handle_command(adapter, &cmd_in);
    }
}

void _netadapter_ts_process_server_socket(netadapter *adapter) {
    net_client tmp_client;
    net_client *p_client;
    int socket;
    socket = accept(adapter->socket, (struct sockaddr *) &tmp_client.addr, &tmp_client.addr_len);
    p_client = netadapter_get_client_by_fd(adapter, socket);
    net_client_init(p_client, socket, tmp_client.addr, tmp_client.addr_len);
    p_client->last_active = clock();

    FD_SET(p_client->socket, &adapter->client_socks);
    printf("Pripojen novy klient(%02d) a pridan do sady socketu\n", p_client->socket);
    return;
}
////  THREAD_SELECT

void *netadapter_thread_select(void *args) {
    netadapter *adapter = (netadapter *) args;
    net_client *p_client;
    int return_value;
    int fd;

    fd_set tests;

    // vyprazdnime sadu deskriptoru a vlozime server socket
    FD_ZERO(&adapter->client_socks);
    FD_SET(adapter->socket, &adapter->client_socks);

    while (adapter->status == NETADAPTER_STATUS_OK) {
        tests = adapter->client_socks;

        // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
        return_value = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
        if (return_value < 0) {
            printf("Select - ERR\n");
            adapter->status = NETADAPTER_STATUS_SELECT_ERROR;
            return NULL;
        }
        // vynechavame stdin, stdout, stderr
        for (fd = NETADAPTER_FD_STD_SKIP; fd < FD_SETSIZE; fd++) {
            // tento socket neni v setu - preskakuje se
            if (!FD_ISSET(fd, &tests)) {
                continue;
            }
            if (fd == adapter->socket) {
                _netadapter_ts_process_server_socket(adapter);
            } else {
                p_client = netadapter_get_client_by_fd(adapter, fd);
                _netadapter_ts_process_client_socket(adapter, p_client);
            }
        }
    }
    printf("Netadapter: finished with status %d.\n", adapter->status);
    
    adapter->status = NETADAPTER_STATUS_FINISHED;
}

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

int netadapter_init(netadapter *p, int port, net_client *clients, int clients_size, short *fd_to_client) {
    memset(p, 0, sizeof (netadapter));

    p->port = port;
    p->clients = clients;
    p->clients_size = clients_size;
    p->fd_to_client = fd_to_client;

    return _netadapter_bind_and_listen(p);
}

void netadapter_shutdown(netadapter *p){
    p->status = NETADAPTER_STATUS_SHUTTING_DOWN;
    
}

////  NETADAPTER - command sending
int netadapter_send_command(net_client *client, network_command *cmd) {
    char buffer[sizeof (network_command) + 2];
    int a2write;
    a2write = network_command_to_string(buffer, cmd);

    memcpy(buffer + a2write, "\n\0", 2); // message footer
    write(client->socket, buffer, a2write + 2);

    network_command_print("Sent", cmd);

    return 0;
}

int netadapter_broadcast_command(net_client *clients, int clients_size, network_command *cmd) {
    int i, counter = 0;
    network_command_print("bc", cmd);
    for (i = 0; i < clients_size; i++) {
        if ((clients + i)->status == NET_CLIENT_STATUS_CONNECTED) {
            netadapter_send_command(clients + i, cmd);
            counter++;
        }
    }

    return counter;
}

//// NETADAPTER - client controls
short _netadapter_first_free_client_offset(netadapter *p) {
    short i;
    for (i = 0; i < p->clients_size; i++) {
        if ((p->clients + i)->status == NET_CLIENT_STATUS_EMPTY) {
            return i;
        }
    }

    return -1;
}

net_client *netadapter_get_client_by_fd(netadapter *p, int fd) {
    short offset;
    if (p->fd_to_client[fd] == NET_CLIENT_ID_EMPTY) {
        offset = _netadapter_first_free_client_offset(p);
        if (offset == -1) {
            return NULL;
        }

        p->fd_to_client[fd] = offset;
    }

    return p->clients + p->fd_to_client[fd];
}

int netadapter_client_aid_by_client(netadapter *adapter, net_client *p_cl) {
    int offset = p_cl - adapter->clients;
    if (offset < 0 || offset >= adapter->clients_size) {
        return -1;
    }
    return offset;
}

void netadapter_check_idle_clients(netadapter *p) {
    int i;
    net_client *p_client;
    clock_t now = clock();
    double sec;

    for (i = 0; i < p->clients_size; i++) {
        p_client = p->clients + i;
        sec = (double) (now - p_client->last_active);
        printf("Client %02d inactive for %02.01f seconds.\n", i, sec);
    }
}
