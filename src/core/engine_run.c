#include <string.h>

#include "engine.h"

#include "../localisation.h"
#include "../logger.h"

char *_engine_cmd_exe_retval_label(int ret_val) {
    switch (ret_val) {
        case ENGINE_CMDEXE_DATA_TOO_SHORT:
            return "Data too short";
        case ENGINE_CMDEXE_ILLEGAL_OP:
            return "Illegal operation";
        case ENGINE_CMDEXE_WRONG_CONTEXT:
            return "Wrong context";
    }
    return "???";
}
char *_engine_close_connection_label(int close_reason) {
    switch(close_reason){
        case ENGINE_CLOSE_REASON_ACTION_NOT_IMPLEMENTED:
            return "Action not implemented";
        case ENGINE_CLOSE_REASON_TOO_MANY_INVALIDES:
            return "Too many invallid messages";
    }
    return "???";
}

int _engine_process_command(engine *p, net_client *p_cli, network_command cmd) {
    int(* handle_action) ENGINE_HANDLE_FUNC_HEADER;
    str_scanner scanner;
    game_room *p_gr;
    int ret_val;

    if (cmd.type < 0 || cmd.type > NETWORK_COMMAND_TYPES_COUNT) {
        network_command_prepare(&p_cli->connection->_out_buffer, NCT_LEAD_DISCONNECT);
        snprintf(p_cli->connection->_out_buffer.data, NETWORK_COMMAND_DATA_LENGTH,
                g_loc.server_protection_illegal_cmd_type, cmd.type);
        engine_send_command(p, p_cli, &p_cli->connection->_out_buffer);
        return ENGINE_CLOSE_REASON_ACTION_NOT_IMPLEMENTED;
    }

    handle_action = p->command_proccess_func[cmd.type];
    if (handle_action == NULL) {
        network_command_prepare(&p_cli->connection->_out_buffer, NCT_LEAD_DISCONNECT);
        snprintf(p_cli->connection->_out_buffer.data, NETWORK_COMMAND_DATA_LENGTH,
                g_loc.server_protection_unimplemented_cmd_type, cmd.type);
        engine_send_command(p, p_cli, &p_cli->connection->_out_buffer);

        return ENGINE_CLOSE_REASON_ACTION_NOT_IMPLEMENTED;
    }

    str_scanner_set(&scanner, cmd.data, cmd.length);
    p_gr = engine_game_room_by_id(p, p_cli->room_id);


    ret_val = handle_action(p, p_cli, &scanner, p_gr);
    if (ret_val) {
        glog(LOG_FINE, "Processed command of type %d from client %d which "
                "resulted in %s", cmd.type, p_cli - p->resources->clients , _engine_cmd_exe_retval_label(ret_val));

        netadapter_handle_invallid_command(p->p_netadapter, p_cli, cmd);
        p->p_netadapter->stats->commands_received_invalid++;
        if (p_cli->connection->invalid_counter > p->p_netadapter->ALLOWED_INVALLID_MSG_COUNT) {
            return ENGINE_CLOSE_REASON_TOO_MANY_INVALIDES;
        }
    }

    return 0;
}

int _engine_authorize_reconnect(engine *p, net_client *p_cli) {
    game_room *p_gr;

    p_gr = engine_game_room_by_id(p, p_cli->room_id);
    if (p_gr != NULL && game_room_find_client(p_gr, p_cli) != ITEM_EMPTY) {
        engine_game_room_put_client(p, p_gr, p_cli);
    }
}

int _engine_authorize_connection(engine *p, int socket, network_command cmd) {
    net_client* p_cli;
    network_command cmd_out;
    my_byte reintroduce;

    if (cmd.type != NCT_LEAD_INTRODUCE) {
        glog(LOG_FINE, "Authorization of connection %02d failed because command"
                "type was not correct. Expected %d, got %d", socket, NCT_LEAD_INTRODUCE, cmd.type);
        return 1;
    }
    if (cmd.length < 2) {
        glog(LOG_FINE, "Authorization of connection %02d failed because command"
                " was too short", socket);
        return 1;
    }

    reintroduce = read_hex_byte(cmd.data);
    if (reintroduce) {
        p_cli = engine_client_by_secret(p, cmd.data + 2);
        if (p_cli == NULL) {
            reintroduce = 0;
        } else {
            if (p_cli->status != NET_CLIENT_STATUS_DISCONNECTED) {
                glog(LOG_FINE, "Reauthorization of connection %02d failed "
                        "because command client is not disconnected", socket);
                return 1;
            }
        }

    }
    if (!reintroduce) {
        p_cli = engine_first_free_client_offset(p);
    }

    if (p_cli == NULL) {
        glog(LOG_FINE, "Authorization of socket %02d failed because there"
                "was no more room for new client", socket);
        return 1;
    }

    p->resources->con_to_cli[socket] = p_cli - p->resources->clients;
    if (!reintroduce) {
        net_client_init(p_cli, p->resources->connections + socket);
        strrand(p_cli->connection_secret, NET_CLIENT_SECRET_LENGTH);
        p_cli->connection_secret[NET_CLIENT_SECRET_LENGTH] = '\0';
        p_cli->room_id = ITEM_EMPTY;
    } else {
        p_cli->connection = p->resources->connections + socket;
    }
    p_cli->status = NET_CLIENT_STATUS_CONNECTED;

    network_command_prepare(&cmd_out, NCT_LEAD_INTRODUCE);
    write_hex_byte(cmd_out.data, reintroduce);
    memcpy(cmd_out.data + 2, p_cli->connection_secret, NET_CLIENT_SECRET_LENGTH);

    netadapter_send_command(p->p_netadapter, p_cli->connection, &cmd_out);

    if (reintroduce) {
        _engine_authorize_reconnect(p, p_cli);
    } else {
        glog(LOG_INFO, "Connection %02d authorized as client %02d", socket, p->resources->con_to_cli[socket]);
    }


    return 0;
}

void _engine_process_queue(engine *p) {
    network_command cmd;
    net_client *p_cli;
    int ret_val;
    char *reason;

    while (!cmd_queue_is_empty(&p->cmd_in_queue)) {
        cmd = cmd_queue_get(&p->cmd_in_queue);
        p_cli = engine_client_by_socket(p, cmd.origin_socket);
        if (p_cli == NULL) {
            ret_val = _engine_authorize_connection(p, cmd.origin_socket, cmd);
            if (ret_val) {
                p->p_netadapter->stats->commands_received_invalid++;
                netadapter_close_connection_by_socket(p->p_netadapter, cmd.origin_socket);
            }
            continue;
        }

        ret_val = _engine_process_command(p, p_cli, cmd);
        if (ret_val) {
            reason = _engine_close_connection_label(ret_val);
            glog(LOG_FINE, "Engine: Closing connection on socket %02d, reason = %s",
                    p_cli->connection->socket, reason);
            netadapter_close_connection_by_client(p->p_netadapter, p_cli);
        }
    }
}

void _engine_check_client_for_idling(engine *p, net_client *p_client, time_t now) {
    tcp_connection *p_con = p_client->connection;
    int idle_time = now - p_con->last_active;

    switch (p_client->status) {
        default:
        case NET_CLIENT_STATUS_CONNECTED:
            if (idle_time > p->netadapter.ALLOWED_IDLE_TIME) {
                network_command_strprep(&p_con->_out_buffer, NCT_LEAD_MARCO, g_loc.netcli_dcreason_unresponsive);
                netadapter_send_command(p->p_netadapter, p_con, &p_con->_out_buffer);
                p_client->status = NET_CLIENT_STATUS_UNRESPONSIVE;
            }
            break;
        case NET_CLIENT_STATUS_UNRESPONSIVE:
            if (idle_time > p->netadapter.ALLOWED_UNRESPONSIVE_TIME) {
                netadapter_close_connection_by_client(p->p_netadapter, p_client);
            }
            break;
    }
}

void _engine_check_active_clients(engine *p) {
    int i;
    net_client *p_cli;
    time_t now = time(NULL);
    
    for (i = 0; i < p->resources->clients_length; i++) {
        p_cli = p->resources->clients + i;

        if (p_cli->connection != NULL) { // connection is open
//            _engine_check_client_for_idling(p, p_cli, now);
        } else { // connection is not open
            if (p_cli->status != NET_CLIENT_STATUS_DISCONNECTED &&
                    p_cli->status != NET_CLIENT_STATUS_EMPTY) {
                glog(LOG_FINE, "Engine: Client %d found to be disconnected", i);
                engine_client_disconnected(p, p_cli, "Client timed out");
            }
        }

    }
}

void *engine_run(void *args) {
    engine *p = (engine *) args;

    p->netadapter.command_handler = p;
    p->netadapter.command_handle_func = &_engine_handle_command;

    p->stats.run_start = clock();
    glog(LOG_INFO, "Engine: Starting");
    while (p->keep_running) {
        _engine_check_active_clients(p);
        _engine_process_queue(p);


        p->total_ticks++;
        if (p->total_ticks > p->settings->MAX_TICKRATE * 30) {

        }
        nanosleep(&p->sleep, NULL);
    }

    p->stats.run_end = clock();
    glog(LOG_INFO, "Engine: Finished");
    netadapter_shutdown(&p->netadapter);
    return NULL;
}