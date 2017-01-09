#include <stdio.h>
#include <string.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include <unistd.h>

#include "netadapter.h"
#include "../localisation.h"
#include "../logger.h"

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

net_client *_netadapter_find_client_by_secret(netadapter *p, char *secret) {
    int i;
    for (i = 0; i < p->clients_length; i++) {
        if (!strcmp((p->clients + i)->connection_secret, secret)) {
            return p->clients + i;
        }
    }

    return NULL;
}

//// thread select file

void _netadapter_handle_invalid_message(netadapter *p, tcp_connection *p_con, char *msg, int msg_len) {
    network_command cmd;
    network_command_prepare(&cmd, NCT_LEAD_BAD_FORMAT);

    network_command_set_data(&cmd, msg, msg_len);

    netadapter_send_command(p, p_con, &cmd);
    p->stats->commands_received_invalid++;

    ++(p_con->invalid_counter);
}
void _netadapter_handle_invalid_command(netadapter *p, tcp_connection *p_con, network_command cmd){
    // TODO: implement
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
    // todo: remove processing messages
    switch (sid->type) {
        case SOCKET_IDENTIFIER_TYPE_TBD:
            printf("Processing uncliented socket %d\n", sid->offset);
            // net_client_init(p_client, connection); todo: pair with client
            return 0;
        case SOCKET_IDENTIFIER_TYPE_CLIENT:
            printf("Processing cliented socket %d\n", sid->offset);
            p->command_handle_func(p->command_handler, cmd);
            return 0;
        default:
            glog(LOG_WARNING, "Attempted to process socket %d which belongs to "
            "neither temporary nor client connection!", sid->offset);
    }
}

int _netadapter_ts_process_remote_socket(netadapter *p, socket_identifier *sid) {
    net_client *p_cli;
    tcp_connection *p_con;
    int ret_val;
    int socket = sid - p->sock_ids;

    int cr_pos; // carriage return - to recognise end of command
    int processed_commands = 0;

    if (netadapter_unpack_sid(p, sid, &p_con, &p_cli) == SOCKET_IDENTIFIER_TYPE_EMPTY) {
        glog(LOG_WARNING, "Netadapter: Could not unpack socket identifier for "
                "socket %d. Terminating connection.", socket);
        return -1;
    }

    ioctl(p_con->socket, FIONREAD, &p_con->a2read);

    // this read cycle might have more chars than buffer can contain
    do {
        ret_val = _netadapter_ts_process_raw_connection(p, p_con);

        switch (ret_val) {
            case NETADAPTER_SOCK_ERROR_NOTHING_TO_READ:
                glog(LOG_INFO, "Netadapter: Connection reset on socket %02d. "
                        "Terminating connection.", p_con->socket);
                return -1;
            case NETADAPTER_SOCK_ERROR_INVALID_MSG_COUNT_EXCEEDED:
                glog(LOG_INFO, "Netadapter: Connection on socket %02d sent too "
                        "many malformed messages. Terminating connection.", p_con->socket);
                return -1;
        }


        glog(LOG_FINE, "Processing message buffer: [%s]", p_con->_in_buffer);
        // multiple commands might have arrived in this read cycle
        while ((cr_pos = strpos(p_con->_in_buffer, "\n")) != STR_NOT_FOUND) {
            if (cr_pos < NETWORK_COMMAND_HEADER_SIZE) {
                glog(LOG_FINE, "Connection on socket %02d sent too short "
                        "message. Terminating connection.", p_con->socket);
                return -1;
            }
            
            glog(LOG_FINE, "Parsing command #%d from [%02d]%s\n",
                    ++processed_commands, cr_pos, p_con->_in_buffer);
            ret_val = network_command_from_string(&p->_cmd_in_buffer, p_con->_in_buffer, cr_pos);
            
            if(!ret_val){
                _netadapter_handle_command(p, p->_cmd_in_buffer, sid);
                p->stats->commands_received++;
            } else {
                _netadapter_handle_invalid_message(p, p_con, p_con->_in_buffer, cr_pos);
                p->stats->commands_received_invalid++;
            }
            
            strshift(p_con->_in_buffer, TCP_CONNECTION_BUFFER_SIZE, cr_pos + 1);
            p_con->_in_buffer_ptr -= (cr_pos + 1);
        }
    } while (p_con->a2read > 0);
    
    return 0;
}

void netadapter_close_connection_msg(netadapter *p, tcp_connection *p_con, const char *msg) {
    network_command_prepare(&p_con->_out_buffer, NCT_LEAD_DENY);
    network_command_set_data(&p_con->_out_buffer, msg, strlen(msg));

    netadapter_send_command(p, p_con, &p_con->_out_buffer);
    close(p_con->socket);
}

void _netadapter_ts_process_server_socket(netadapter *adapter) {
    tcp_connection *p_con;
    tcp_connection tmp_con;

    // todo: closing server socket does not alert the select, so a timeout
    // has been implemented as a quick-fix, look into it later pls
    if (adapter->status != NETADAPTER_STATUS_OK) {
        glog(LOG_INFO, "Server socket is not being processed as netadapter is not ok.");
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
    glog(LOG_INFO, "New connection on socket %02d has been added "
            "to socket set.", p_con->socket);

    return;
}
////  THREAD_SELECT

void *netadapter_thread_select(void *args) {
    netadapter *adapter = (netadapter *) args;
    socket_identifier *sid;
    int ret_val;
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

        // sada deskriptoru je po kazdem volani select prepsana
        // sadou deskriptoru kde se neco delo
        ret_val = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, &select_timeout);
        if (ret_val < 0) {
            fprintf(stderr, "Select error no.: %d", ret_val);
            glog(LOG_ERROR, "Error at select(), errno: %d", ret_val);
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
    glog(LOG_INFO, "Netadapter: finished with status %d.", adapter->status);
    adapter->status = NETADAPTER_STATUS_FINISHED;
    
    return NULL;
}
