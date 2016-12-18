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
    for (i = 0; i < p->clients_length; i++) {
        if ((p->clients + i)->connection.status == TCP_CONNECTION_STATUS_EMPTY) {
            return i;
        }
    }

    return -1;
}

tcp_connection *_netadapter_first_free_connection(netadapter *p) {
    int i;
    for (i = 0; i < p->connections_length; i++) {
        if ((p->connections + i)->status == TCP_CONNECTION_STATUS_EMPTY) {
            return p->connections + i;
        }
    }

    return NULL;
}

//// thread select file

void _netadapter_handle_invalid_message(netadapter *adapter, tcp_connection *p_con, char *msg, int msg_len) {
    network_command cmd;
    network_command_prepare(&cmd, NCT_LEAD_BAD_FORMAT);

    network_command_set_data(&cmd, msg, msg_len);

    netadapter_send_command(p_con, &cmd);

    ++(p_con->invalid_counter);
}

/**
 * Tries to read from socket on provided connection and transform incomming
 * message into command in provided netadapter's command buffer.
 * If a valid message can be read, connection in question gets its last_active
 * mark renewed.
 * 
 * @param p netadapter underneath which is connection is handled
 * @param p_con connection in question
 * @return 0 if ok or negative error code identificator
 */
int _netadapter_ts_process_raw_connection(netadapter *p, tcp_connection *p_con) {
    int read_size;

    memset(p->_buffer, 0, NETADAPTER_BUFFER_SIZE);

    if (p_con->a2read <= 0) {
        return NETADAPTER_SOCK_ERROR_NOTHING_TO_READ;
    }

    // how many chars can still be stuffed into buffer
    read_size = NETADAPTER_BUFFER_SIZE - 2 - p_con->_in_buffer_ptr;
    if (p_con->a2read < read_size) {
        read_size = p_con->a2read;
    }
    if (read_size < 1) {
        return NETADAPTER_SOCK_ERROR_MSG_TOO_LONG;
    }

    // read available chars
    read(p_con->socket, p_con->_in_buffer + p_con->_in_buffer_ptr, read_size);
    p_con->a2read -= read_size;
    p_con->_in_buffer_ptr += read_size;

    p_con->last_active = time(NULL);

    return 0;
}

int _netadapter_handle_command(netadapter *p, network_command cmd, socket_identifier *sid) {
    cmd.remote_identifier = sid->offset;
    switch (sid->type) {
        case SOCKET_IDENTIFIER_TYPE_TBD:
            printf("Processing uncliented socket %d", sid->offset);
            // net_client_init(p_client, connection); todo: pair with client
            return 0;
        case SOCKET_IDENTIFIER_TYPE_CLIENT:
            printf("Processing cliented socket %d", sid->offset);
            p->command_handle_func(p->command_handler, cmd);
            return 0;
    }
}

int _netadapter_ts_process_remote_socket(netadapter *p, socket_identifier *sid) {
    net_client *p_cli;
    tcp_connection *p_con;
    int ret_val;
    int socket = sid - p->sock_ids;

    int cr_pos;

    if (sid->type == SOCKET_IDENTIFIER_TYPE_EMPTY) {
        printf("TS-ERR: Attempted to process socket %d but its identifier "
                "is empty.\n", socket);
        return -1;
    }

    if (netadapter_unpack_sid(p, sid, &p_con, &p_cli) == SOCKET_IDENTIFIER_TYPE_EMPTY) {
        printf("SID unpack fail\n");
        return -1;
    }

    ioctl(p_con->socket, FIONREAD, &p_con->a2read);

    // this read cycle might have more chars than buffer can contain
    do {
        ret_val = _netadapter_ts_process_raw_connection(p, p_con);

        switch (ret_val) {
            case NETADAPTER_SOCK_ERROR_NOTHING_TO_READ:
                printf("Netadapter: something wrong happened on socket %02d. "
                        "They will be put down now.\n", p_con->socket);
                return -1;
            case NETADAPTER_SOCK_ERROR_INVALID_MSG_COUNT_EXCEEDED:
                printf("Netadapter: connection on socket %02d kept sending "
                        "gibberish. It will be terminated soon.\n", p_con->socket);
                return -1;
        }

        // multiple commands might have arrived in this read cycle
        while ((cr_pos = strpos(p_con->_in_buffer, "\n") != STR_NOT_FOUND)) {
            network_command_from_string(&p->_cmd_in_buffer, p_con->_in_buffer, cr_pos);
            _netadapter_handle_command(p, p->_cmd_in_buffer, sid);
        }

        return 0;
    } while (p_con->a2read > 0);
}

void netadapter_close_connection_msg(netadapter *p, tcp_connection *p_con, const char *msg) {
    network_command_prepare(&p_con->_out_buffer, NCT_LEAD_DENY);
    network_command_set_data(&p_con->_out_buffer, msg, strlen(msg));

    netadapter_send_command(p_con, &p_con->_out_buffer);
    close(p_con->socket);
}

void _netadapter_ts_process_server_socket(netadapter *adapter) {
    tcp_connection *p_con;
    tcp_connection tmp_con;

    printf("Processign server socket\n");

    if (adapter->status != NETADAPTER_STATUS_OK) {
        printf("Server socket is not being processed as netadapter is not ok.");
        return;
    }

    p_con = _netadapter_first_free_connection(adapter);

    if (p_con == NULL) {
        p_con = &tmp_con;
    }

    memset(p_con, 0, sizeof (tcp_connection));

    p_con->socket = accept(adapter->socket, (struct sockaddr *) &p_con->addr, &p_con->addr_len);

    if (p_con == &tmp_con) {
        netadapter_close_connection_msg(adapter, p_con, loc.socket_reject_no_room);
        return;
    }

    if (netadapter_set_sid(adapter, p_con->socket, SOCKET_IDENTIFIER_TYPE_TBD, p_con - adapter->connections) != 0) {
        netadapter_close_connection_msg(adapter, p_con, loc.socket_reject_invalid_number);
        return;
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
    struct timeval select_timeout;

    // vyprazdnime sadu deskriptoru a vlozime server socket
    FD_ZERO(&adapter->client_socks);
    FD_SET(adapter->socket, &adapter->client_socks);

    while (adapter->status == NETADAPTER_STATUS_OK) {
        tests = adapter->client_socks;

        select_timeout.tv_sec = 1;
        select_timeout.tv_usec = 0;

        // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
        return_value = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, &select_timeout);
        if (return_value < 0) {
            printf("Select - ERR:%d\n", return_value);
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
//                if (sid == NULL) {
//                    printf("TS-ERR: Attempted to process socket %d which is out of "
//                            "identifier space.\n", fd);
//                }
                if (_netadapter_ts_process_remote_socket(adapter, sid) < 0) {
                    netadapter_close_socket_by_sid(adapter, sid);
                }
            }
        }
    }
    printf("Netadapter: finished with status %d.\n", adapter->status);
    adapter->status = NETADAPTER_STATUS_FINISHED;
}
