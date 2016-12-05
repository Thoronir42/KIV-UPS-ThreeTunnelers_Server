#include <stdio.h>
#include <string.h>
// kvuli iotctl
#include <sys/ioctl.h>

#include "netadapter.h"

//// accessors


int _netadapter_first_free_client_offset(netadapter *p) {
    int i;
    for (i = 0; i < p->clients_size; i++) {
        if ((p->clients + i)->status == NET_CLIENT_STATUS_EMPTY) {
            return i;
        }
    }

    return -1;
}

client_connection *_netadapter_first_free_connection(netadapter *p) {
    int i;
    for (i = 0; i < NETADAPTER_CONNECTIONS_RESERVE; i++) {
        if ((p->connections + i)->socket == CLIENT_CONNECTION_EMPTY_SOCKET) {
            return p->connections + i;
        }
    }
    return NULL;
}

//// thread select file

int _netadapter_process_message(int socket, char *buffer, int read_size, network_command *command) {
    int length;

    length = NETADAPTER_BUFFER_SIZE - 2;
    if (read_size < length) {
        length = read_size;
    }

    read(socket, buffer, length);
    return network_command_from_string(command, buffer, read_size);

}

int _netadapter_handle_invalid_message(netadapter *adapter, client_connection *p_con) {
    network_command cmd;
    memset(&cmd, 0, sizeof (network_command));

    cmd.type = NET_CMD_LEAD_BAD_FORMAT;
    read(p_con->socket, cmd.data, p_con->a2read);
    netadapter_send_command(p_con, &cmd);

    return ++(p_con->invalid_counter);
}

void _netadapter_ts_process_client_socket(netadapter *p, int socket) {

}

void _netadapter_ts_process_socket(netadapter *p, int socket) {
    socket_identifier *sid = netadapter_get_sid_by_socket(p, socket);
    net_client *p_cli;
    client_connection *p_con;

    if (sid == NULL) {
        printf("TS-ERR: Attempted to process socket %d which is out of "
                "identifier space.\n", socket);
    }
    
    switch (sid->type) {
        case SOCKET_IDENTIFIER_TYPE_EMPTY:
            printf("TS-ERR: Attempted to process socket %d but its identifier "
                    "is empty.\n", socket);
            return;

        case SOCKET_IDENTIFIER_TYPE_TBD:
            printf("Processing uncliented socket %d", socket);
            p_con = p->connections + sid->offset;
            // net_client_init(p_client, connection); todo: pair with client
            break;
        case SOCKET_IDENTIFIER_TYPE_CLIENT:
            printf("Processing client socket %d", socket);
            p_cli = p->clients + sid->offset;
            p_con = &p_cli->connection;
            break;
    }
    p_con->last_active = time(NULL);

    memset(p->_buffer, 0, NETADAPTER_BUFFER_SIZE);

    ioctl(p_con->socket, FIONREAD, &p_con->a2read);
    if (p_con->a2read < NETWORK_COMMAND_HEADER_SIZE) { // mame co cist
        if (p_con->a2read <= 0) {
            printf("Netadapter: something wrong happened on socket %02d. "
                    "They will be put down now.\n", p_con->socket);
            netadapter_close_socket_by_sid(p, sid);
            return;
        }

        if (_netadapter_handle_invalid_message(p, p_con) > p->ALLOWED_INVALLID_MSG_COUNT) {
            printf("Netadapter: client on socket %02d kept sending gibberish. They "
                    "will be terminated immediately.\n", p_con->socket);
            netadapter_close_socket_by_sid(p, sid);
        }

    } else {
        p_con->last_active = time(NULL);
        _netadapter_process_message(p_con->socket, p->_buffer, p_con->a2read, &p->_cmd_in_buffer);
        p->_cmd_in_buffer.client_aid = netadapter_client_aid_by_client(p, p_cli); // fixme: client_aid

        p->command_handle_func(p->command_handler, p->_cmd_in_buffer);
    }
}

void _netadapter_reject_full(int socket){
    
}

void _netadapter_ts_process_server_socket(netadapter *adapter) {
    client_connection *p_con;
    
    p_con = _netadapter_first_free_connection(adapter);
    
    if(p_con == NULL){
        _netadapter_reject_full(adapter->socket);
        return;
    }
    
    memset(p_con, 0, sizeof (client_connection));

    p_con->socket = accept(adapter->socket, (struct sockaddr *) &p_con->addr, &p_con->addr_len);
    if(p_con->socket > adapter->sock_ids_length){
        
    }
    
    p_con->last_active = time(NULL);

    FD_SET(p_con->socket, &adapter->client_socks);
    printf("Pripojen novy klient(%02d) a pridan do sady socketu\n", p_con->socket);
    return;
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
            // tento socket neni v setu - preskakuje se
            if (!FD_ISSET(fd, &tests)) {
                continue;
            }
            if (fd == adapter->socket) {
                _netadapter_ts_process_server_socket(adapter);
            } else {
                _netadapter_ts_process_socket(adapter, fd);
            }
        }
    }
    printf("Netadapter: finished with status %d.\n", adapter->status);

    adapter->status = NETADAPTER_STATUS_FINISHED;
}