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

////  THREAD_SELECT - PROCESS INCOMMING STUFF

int netadapter_process_message(net_client *p_client, char *buffer, int read_size, network_command *command) {
    int length;

    length = NETADAPTER_BUFFER_SIZE - 2;
    if (read_size < length) {
        length = read_size;
    }

    read(p_client->socket, buffer, length);
    return network_command_from_string(command, buffer);

}

void netadapter_thread_select_handle_command(netadapter *p, const network_command *cmd_in) {
    network_command cmd_out;
    net_client *p_client = (p->clients + cmd_in->client_aid);

    switch (cmd_in->type) {
        default:
            cmd_out.type = NET_CMD_UNDEFINED;
            memcpy(cmd_out.data, "Cmd type unrecognised\0", 22);
            netadapter_send_command(p_client, &cmd_out);
            break;
        case NET_CMD_MSG_RCON:
            cmd_out.id = 66;
            memset(cmd_out.data, 0, NETWORK_COMMAND_DATA_LENGTH);
            strrev(cmd_out.data, cmd_in->data, cmd_in->length);

            netadapter_send_command(p_client, &cmd_out);
            break;
        case NET_CMD_MSG_PLAIN:
            cmd_out.type = NET_CMD_MSG_PLAIN;
            memcpy(cmd_out.data, cmd_in->data, cmd_in->length);
            cmd_out.data[cmd_in->length] = '\0';
            netadapter_broadcast_command(p->clients, p->clients_size, &cmd_out);
            break;
    }
}

void netadapter_thread_select_process_socket(netadapter *adapter, int fd) {
    net_client tmp_client;
    net_client *p_client;
    network_command cmd_in;
    int socket;


    memset(adapter->_buffer, 0, NETADAPTER_BUFFER_SIZE);
    // je to server socket ? prijmeme nove spojeni
    if (fd == adapter->socket) {
        socket = accept(adapter->socket, (struct sockaddr *) &tmp_client.addr, &tmp_client.addr_len);
        p_client = adapter->clients - 3 + socket; // std -in -out -err, server socket
        net_client_init(p_client, socket);

        FD_SET(p_client->socket, &adapter->client_socks);
        printf("Pripojen novy klient(%02d) a pridan do sady socketu\n", p_client->socket);
        return;
    } else {
        p_client = adapter->clients - 3 + fd; // fixme: array out of boudns?
    }

    // je to klientsky socket ? prijmem data
    // pocet bajtu co je pripraveno ke cteni
    ioctl(fd, FIONREAD, &p_client->a2read);
    if (p_client->a2read < NETWORK_COMMAND_HEADER_SIZE) { // mame co cist
        if (p_client->a2read > 0) {
            memset(&cmd_in, 0, sizeof (network_command));
            cmd_in.type = NET_CMD_BAD_FORMAT;
            read(fd, cmd_in.data, p_client->a2read);
            netadapter_send_command(p_client, &cmd_in);
        }
        close(fd);
        FD_CLR(fd, &adapter->client_socks);
        printf("Klient(%02d) se odpojil a byl odebran ze sady socketu\n", fd);
    } else {
        netadapter_process_message(p_client, adapter->_buffer, p_client->a2read, &cmd_in);
        cmd_in.client_aid = fd - 3; // fixme: client_aid
        network_command_print("Received", &cmd_in);
        netadapter_thread_select_handle_command(adapter, &cmd_in);
    }
}

////  THREAD_SELECT

void *netadapter_thread_select(void *args) {
    netadapter *adapter = (netadapter *) args;
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
            // je dany socket v sade fd ze kterych lze cist ?
            if (FD_ISSET(fd, &tests)) {
                netadapter_thread_select_process_socket(adapter, fd);
            }
        }
    }
    adapter->status = NETADAPTER_STATUS_FINISHED;
}

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

