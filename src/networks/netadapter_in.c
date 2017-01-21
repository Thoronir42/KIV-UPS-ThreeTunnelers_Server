#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "netadapter.h"
#include "../localisation.h"
#include "../logger.h"

////  accessors

////  THREAD_SELECT

void _netadapter_handle_invalid_message(netadapter *p, tcp_connection *p_con) {
    p->stats->commands_received_invalid++;
    p_con->invalid_counter++;
}

void netadapter_handle_invallid_command(netadapter *p, net_client *p_cli, network_command cmd) {
    _netadapter_handle_invalid_message(p, p_cli->connection);
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
int _netadapter_ts_process_raw_connection(netadapter *p, tcp_connection * p_con) {
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

    p->stats->bytes_received += read_size;
    
//    glog(LOG_INFO, "Connection %d buffer status is [%03d/%03d] (%d new)",
//            p_con->socket, p_con->_in_buffer_ptr, TCP_CONNECTION_BUFFER_SIZE, read_size);

    return 0;
}

/**
 * TODO: refactor this huge beast of a function
 * @param p
 * @param socket
 * @return 
 */
int _netadapter_ts_process_remote_socket(netadapter *p, int socket) {
    tcp_connection *p_con;
    int ret_val;

    int lf_pos; // line feed - to recognise end of command
    int processed_commands = 0;

    p_con = p->connections + socket;

    ioctl(p_con->socket, FIONREAD, &p_con->a2read);

    // this read cycle might have more chars than buffer can contain
    do {
        ret_val = _netadapter_ts_process_raw_connection(p, p_con);
        if (ret_val) {
            return ret_val;
        }

//        glog(LOG_FINE, "Processing socket %03d message buffer: [%s]", socket, p_con->_in_buffer);
        
        // multiple commands might have arrived in this read cycle
        lf_pos = lf_pos = strpos(p_con->_in_buffer, "\n");
        if (lf_pos == STR_NOT_FOUND) {
//            glog(LOG_FINE, "Socket %d: No line feed found", socket);
            break;
        } else {
//            glog(LOG_FINE, "Socket %d: Line feed at %d", socket, lf_pos);
        }


        do {
            if (lf_pos < NETWORK_COMMAND_HEADER_SIZE) {
                return NETADAPTER_SOCK_ERROR_MSG_TOO_SHORT;
            }
            p_con->_in_buffer[lf_pos] = '\0';
//            glog(LOG_FINE, "Parsing command #%d long %d characters \"%s\"",
//                    ++processed_commands, lf_pos, p_con->_in_buffer);
            ret_val = network_command_from_string(&p->_cmd_in_buffer, p_con->_in_buffer, lf_pos);

            if (!ret_val) {
                p->stats->commands_received++;

                p->_cmd_in_buffer.origin_socket = socket;
//                glog(LOG_FINE, "Passing command from socket %d \"%04X:%s\"",
//                        p->_cmd_in_buffer.origin_socket, p->_cmd_in_buffer.type,
//                        p->_cmd_in_buffer.data);
                p->command_handle_func(p->command_handler, p->_cmd_in_buffer);
            } else {
                _netadapter_handle_invalid_message(p, p_con);
                p->stats->commands_received_invalid++;
            }
            if (p_con->invalid_counter > p->ALLOWED_INVALLID_MSG_COUNT) {
                return NETADAPTER_SOCK_ERROR_INVALID_MSG_COUNT_EXCEEDED;
            }

            strshift(p_con->_in_buffer, TCP_CONNECTION_BUFFER_SIZE, lf_pos + 1);
            p_con->_in_buffer_ptr -= (lf_pos + 1);
        } while ((lf_pos = strpos(p_con->_in_buffer, "\n")) != STR_NOT_FOUND);
    } while (p_con->a2read > 0);

    return 0;
}

void _netadapter_log_socket_error(int socket, int error_number) {
    switch (error_number) {
        case NETADAPTER_SOCK_ERROR_MSG_TOO_LONG:
            glog(LOG_FINE, "Connection on socket %03d buffer overflow."
                    " Terminating connection.", socket);
            break;
        case NETADAPTER_SOCK_ERROR_MSG_TOO_SHORT:
            glog(LOG_FINE, "Connection on socket %03d sent too short "
                    "message. Terminating connection.", socket);
            break;
        case NETADAPTER_SOCK_ERROR_NOTHING_TO_READ:
            glog(LOG_INFO, "Netadapter: Connection reset on socket %02d. "
                    "Terminating connection.", socket);
            break;
        case NETADAPTER_SOCK_ERROR_INVALID_MSG_COUNT_EXCEEDED:
            glog(LOG_INFO, "Netadapter: Connection on socket %02d sent too "
                    "many malformed messages. Terminating connection.", socket);
            break;
        default:
            glog(LOG_WARNING, "Unknown error on socket %02d. "
                    "Terminating connection.", socket);
            break;
    }
}

void _netadapter_ts_process_server_socket(netadapter * p) {
    tcp_connection *p_con;
    tcp_connection tmp_con;

    // todo: closing server socket does not alert the select, so a timeout
    // has been implemented as a quick-fix, look into it later pls
    if (p->status != NETADAPTER_STATUS_OK) {
        glog(LOG_WARNING, "Server socket is not being processed as netadapter is not ok.");
        return;
    }

    memset(&tmp_con, 0, sizeof (tcp_connection));

    tmp_con.socket = accept(p->socket, (struct sockaddr *) &tmp_con.addr, &tmp_con.addr_len);

    if (tmp_con.socket >= p->connections_length) {
        netadapter_close_connection_msg(p, p_con, g_loc.socket_reject_invalid_number);
        return;
    }

    p_con = p->connections + tmp_con.socket;
    *p_con = tmp_con;

    p_con->last_active = time(NULL);

    FD_SET(p_con->socket, &p->client_socks);
    glog(LOG_INFO, "New connection on socket %02d has been added "
            "to socket set.", p_con->socket);

    return;
}

void *netadapter_thread_select(void *args) {
    netadapter *adapter = (netadapter *) args;
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
                ret_val = _netadapter_ts_process_remote_socket(adapter, fd);
                if (ret_val < 0) {
                    _netadapter_log_socket_error(fd, ret_val);
                    netadapter_close_connection_by_socket(adapter, fd);
                }
            }
        }
    }
    char *status_label;
    switch (adapter->status) {
        case NETADAPTER_STATUS_SHUTTING_DOWN:
            status_label = "Shutting down";
            break;
        case NETADAPTER_STATUS_BIND_ERROR:
            status_label = "Bind error";
            break;
        case NETADAPTER_STATUS_LISTEN_ERROR:
            status_label = "Listen error";
            break;
        case NETADAPTER_STATUS_SELECT_ERROR:
            status_label = "Select error";
            break;
        default:
            status_label = "???";
            break;
    }

    glog(LOG_INFO, "Netadapter: finished with status %d: %s.", adapter->status, status_label);
    adapter->status = NETADAPTER_STATUS_FINISHED;

    return NULL;
}
