#include <stdio.h>
#include <string.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include <unistd.h>

#include "netadapter.h"
#include "../localisation.h"

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

tcp_connection *_netadapter_first_free_connection(netadapter *p) {
    int i;
    for (i = 0; i < NETADAPTER_CONNECTIONS_RESERVE; i++) {
        if ((p->connections + i)->socket == CLIENT_CONNECTION_EMPTY_SOCKET) {
            return p->connections + i;
        }
    }
    return NULL;
}

//// thread select file

int _netadapter_process_socket(int socket, char *buffer, int read_size, network_command *command) {
    int length;

    length = NETADAPTER_BUFFER_SIZE - 2;
    if (read_size < length) {
        length = read_size;
    }

    read(socket, buffer, length);

    return network_command_from_string(command, buffer, read_size);
}

void _netadapter_handle_invalid_message(netadapter *adapter, tcp_connection *p_con) {
    network_command cmd;
    memset(&cmd, 0, sizeof (network_command));

    cmd.type = NET_CMD_LEAD_BAD_FORMAT;
    read(p_con->socket, cmd.data, p_con->a2read);
    netadapter_send_command(p_con, &cmd);

    ++(p_con->invalid_counter);
}

void _netadapter_ts_process_client_socket(netadapter *p, int socket) {

}

int _netadapter_ts_process_remote_socket(netadapter *p, socket_identifier *sid) {
    net_client *p_cli;
    tcp_connection *p_con;
    if (sid == NULL) {
        printf("TS-ERR: Attempted to process socket %d which is out of "
                "identifier space.\n", socket);
        return -1;
    }
    if (sid->type == SOCKET_IDENTIFIER_TYPE_EMPTY) {
        printf("TS-ERR: Attempted to process socket %d but its identifier "
                "is empty.\n", socket);
        return -1;
    }

    if (netadapter_unpack_sid(p, sid, &p_con, &p_cli) == SOCKET_IDENTIFIER_TYPE_EMPTY) {
        printf("SID unpack fail");
        return -1;
    }

    p_con->last_active = time(NULL);

    memset(p->_buffer, 0, NETADAPTER_BUFFER_SIZE);

    ioctl(p_con->socket, FIONREAD, &p_con->a2read);

    if (p_con->a2read <= 0) {
        printf("Netadapter: something wrong happened on socket %02d. "
                "They will be put down now.\n", p_con->socket);
        return -1;
    }
    
    if (p_con->a2read < NETWORK_COMMAND_HEADER_SIZE) { // mame co cist
        _netadapter_handle_invalid_message(p, p_con);
        if (p_con->invalid_counter > p->ALLOWED_INVALLID_MSG_COUNT) {
            printf("Netadapter: client on socket %02d kept sending gibberish. They "
                    "will be terminated immediately.\n", p_con->socket);
            return -1;
        }
    }

    _netadapter_process_socket(p_con->socket, p->_buffer, p_con->a2read, &p->_cmd_in_buffer);

    p->_cmd_in_buffer.remote_identifier = sid->offset;

    switch (sid->type) {
        case SOCKET_IDENTIFIER_TYPE_TBD:
            printf("Processing uncliented socket %d", socket);
            // net_client_init(p_client, connection); todo: pair with client
            break;
        case SOCKET_IDENTIFIER_TYPE_CLIENT:
            p->command_handle_func(p->command_handler, p->_cmd_in_buffer);
            break;
    }
}

void _netadapter_reject_msg(int socket, const char *msg) {
    network_command cmd;
    network_command_prepare(&cmd, NET_CMD_LEAD_DENY);
    tcp_connection con;
    if(msg == NULL){
        msg = "Connection refused";
    }
    
    network_command_set_data(&cmd, msg, strlen(msg));
    
    con.socket = accept(socket, (struct sockaddr *) &con.addr, &con.addr_len);
    netadapter_send_command(&con, &cmd);
    close(con.socket);
}

void _netadapter_ts_process_server_socket(netadapter *adapter) {
    tcp_connection *p_con;

    p_con = _netadapter_first_free_connection(adapter);

    if (p_con == NULL) {
        _netadapter_reject_msg(adapter->socket, loc.socket_reject_no_room);
        return;
    }

    memset(p_con, 0, sizeof (tcp_connection));

    p_con->socket = accept(adapter->socket, (struct sockaddr *) &p_con->addr, &p_con->addr_len);
    if (p_con->socket > adapter->sock_ids_length) {

    }

    p_con->last_active = time(NULL);

    FD_SET(p_con->socket, &adapter->client_socks);
    printf("Pripojen novy klient(%02d) a pridan do sady socketu\n", p_con->socket);
    return;
}
////  THREAD_SELECT

void *netadapter_thread_select(void *args) {
    netadapter *adapter = (netadapter *) args;
    socket_identifier *sid;
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
                sid = netadapter_get_sid_by_socket(adapter, fd);
                if (_netadapter_ts_process_remote_socket(adapter, sid) < 0) {
                    netadapter_close_socket_by_sid(adapter, sid);
                }
            }
        }
    }
    printf("Netadapter: finished with status %d.\n", adapter->status);

    adapter->status = NETADAPTER_STATUS_FINISHED;
}