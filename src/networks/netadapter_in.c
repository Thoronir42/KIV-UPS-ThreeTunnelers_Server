#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "netadapter.h"
#include "../localisation.h"
#include "../logger.h"

////  accessors

int _netadapter_first_free_client_offset(netadapter *p) {
    int i;
    for (i = 0; i < p->clients_length; i++) {
        if ((p->clients + i)->status == NET_CLIENT_STATUS_EMPTY) {
            return i;
        }
    }

    return NETADAPTER_ITEM_EMPTY;
}

int _netadapter_find_client_by_secret(netadapter *p, char *secret) {
    int i;
    for (i = 0; i < p->clients_length; i++) {
        if (!strcmp((p->clients + i)->connection_secret, secret)) {
            i;
        }
    }

    return NETADAPTER_ITEM_EMPTY;
}

////  THREAD_SELECT

void _netadapter_handle_invalid_message(netadapter *p, tcp_connection *p_con, char *msg, int msg_len) {
    network_command cmd;
    network_command_prepare(&cmd, NCT_LEAD_BAD_FORMAT);

    network_command_set_data(&cmd, msg, msg_len);

    netadapter_send_command(p, p_con, &cmd);
    p->stats->commands_received_invalid++;

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

int _netadapter_authorize_connection(netadapter *p, int connection_offset, network_command cmd) {
    int client_offset;
    net_client* p_cli;
    network_command cmd_out;
    my_byte reintroduce;
    
    if (cmd.type != NCT_LEAD_INTRODUCE || cmd._length < 2) {
        glog(LOG_FINE, "Authorization of connection %02d failed because command"
                "type was not correct. Expected %d, got %d", connection_offset, NCT_LEAD_INTRODUCE, cmd.type);
        return 1;
    }

    reintroduce = read_hex_byte(cmd.data);
    if (reintroduce) {
        client_offset = _netadapter_find_client_by_secret(p, cmd.data + 2);
        if(client_offset == NETADAPTER_ITEM_EMPTY){
            client_offset = _netadapter_first_free_client_offset(p);
            reintroduce = 0;
        }
    } else {
        client_offset = _netadapter_first_free_client_offset(p);
    }

    if (client_offset == NETADAPTER_ITEM_EMPTY) {
        glog(LOG_FINE, "Authorization of connection %02d failed because there"
                "was no more room for new client", connection_offset);
        return 1;
    }

    p->connection_to_client[connection_offset] = client_offset;

    p_cli = (p->clients + client_offset);
    p_cli->connection = p->connections + connection_offset;
    p_cli->status = NET_CLIENT_STATUS_CONNECTED;

    if(!reintroduce){
        strrand(p_cli->connection_secret, NET_CLIENT_SECRET_LENGTH);
        p_cli->connection_secret[NET_CLIENT_SECRET_LENGTH] = '\0';
    }

    network_command_prepare(&cmd_out, NCT_LEAD_INTRODUCE);
    write_hex_byte(cmd_out.data, reintroduce);
    memcpy(cmd_out.data + 2, p_cli->connection_secret, NET_CLIENT_SECRET_LENGTH);
    

    netadapter_send_command(p, p_cli->connection, &cmd_out);

    glog(LOG_INFO, "Connection %02d authorized as client %02d", connection_offset, client_offset);
    return 0;
}

int _netadapter_pass_command(netadapter *p, network_command cmd) {
    glog(LOG_FINE, "Passing command to %d", cmd.client_aid);
    return p->command_handle_func(p->command_handler, cmd);
}

/**
 * TODO: refactor this huge beast of a function
 * @param p
 * @param socket
 * @return 
 */
int _netadapter_ts_process_remote_socket(netadapter *p, int socket) {
    net_client *p_cli;
    tcp_connection *p_con;
    int ret_val;

    int lf_pos; // line feed - to recognise end of command
    int processed_commands = 0;

    // fixme: add check if socket is valid
    p_con = p->connections + socket;
    if (p_con->client_aid != NETADAPTER_ITEM_EMPTY) {
        p_cli = p->clients + p_con->client_aid;
    }

    ioctl(p_con->socket, FIONREAD, &p_con->a2read);

    // this read cycle might have more chars than buffer can contain
    do {
        ret_val = _netadapter_ts_process_raw_connection(p, p_con);

        switch (ret_val) {
            case NETADAPTER_SOCK_ERROR_NOTHING_TO_READ:
                glog(LOG_INFO, "Netadapter: Connection reset on socket %02d. "
                        "Terminating connection.", p_con->socket);
                return ret_val;
            case NETADAPTER_SOCK_ERROR_INVALID_MSG_COUNT_EXCEEDED:
                glog(LOG_INFO, "Netadapter: Connection on socket %02d sent too "
                        "many malformed messages. Terminating connection.", p_con->socket);
                return ret_val;
            default: break;
        }


        glog(LOG_FINE, "Processing socket %03d message buffer: [%s]", socket, p_con->_in_buffer);
        // multiple commands might have arrived in this read cycle
        while ((lf_pos = strpos(p_con->_in_buffer, "\n")) != STR_NOT_FOUND) {
            if (lf_pos < NETWORK_COMMAND_HEADER_SIZE) {
                glog(LOG_FINE, "Connection on socket %03d sent too short "
                        "message. Terminating connection.", p_con->socket);
                return NETADAPTER_SOCK_ERROR_MSG_TOO_SHORT;
            }
            p_con->_in_buffer[lf_pos] = '\0';
            glog(LOG_FINE, "Parsing command #%d long %d characters \"%s\"",
                    ++processed_commands, lf_pos - 1, p_con->_in_buffer);
            ret_val = network_command_from_string(&p->_cmd_in_buffer, p_con->_in_buffer, lf_pos - 1);

            if (!ret_val) {
                p->_cmd_in_buffer.client_aid = netadapter_client_aid_by_socket(p, socket);
                if (p->_cmd_in_buffer.client_aid != NETADAPTER_ITEM_EMPTY) {
                    if (_netadapter_pass_command(p, p->_cmd_in_buffer)) {
                        _netadapter_handle_invalid_message(p, p_con, p_con->_in_buffer, lf_pos - 1);
                        p->stats->commands_received_invalid++;
                    } else {
                        p->stats->commands_received++;
                    }
                } else {
                    if (_netadapter_authorize_connection(p, socket, p->_cmd_in_buffer)) {
                        glog(LOG_INFO, "Connection authorization failed on "
                                "socket %d. Terminating connection", socket);
                        p->stats->commands_received_invalid++;
                        return NETADAPTER_SOCK_ERROR_AUTHORIZATION_FAIL;
                    } else {
                        p->stats->commands_received++;
                    }
                }
            } else {
                _netadapter_handle_invalid_message(p, p_con, p_con->_in_buffer, lf_pos);
                p->stats->commands_received_invalid++;
            }
            if (p_con->invalid_counter > p->ALLOWED_INVALLID_MSG_COUNT) {
                return NETADAPTER_SOCK_ERROR_INVALID_MSG_COUNT_EXCEEDED;
            }

            strshift(p_con->_in_buffer, TCP_CONNECTION_BUFFER_SIZE, lf_pos + 1);
            p_con->_in_buffer_ptr -= (lf_pos + 1);
        }
    } while (p_con->a2read > 0);

    return 0;
}

void _netadapter_ts_process_server_socket(netadapter *p) {
    tcp_connection *p_con;
    tcp_connection tmp_con;

    // todo: closing server socket does not alert the select, so a timeout
    // has been implemented as a quick-fix, look into it later pls
    if (p->status != NETADAPTER_STATUS_OK) {
        glog(LOG_INFO, "Server socket is not being processed as netadapter is not ok.");
        return;
    }



    memset(&tmp_con, 0, sizeof (tcp_connection));

    tmp_con.socket = accept(p->socket, (struct sockaddr *) &tmp_con.addr, &tmp_con.addr_len);

    if (tmp_con.socket >= p->connections_length) {
        netadapter_close_connection_msg(p, p_con, loc.socket_reject_invalid_number);
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
