#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "engine.h"
#include "resources.h"

#include "../networks/netadapter.h"

#include "../logger.h"
#include "../settings.h"
#include "../statistics.h"
#include "../game/control_input.h"
#include "../structures/cmd_queue.h"
#include "../localisation.h"

#define CLOCK CLOCK_MONOTONIC

int engine_init(engine *p_engine, settings *p_settings, resources *p_resources) {
    memset(p_engine, 0, sizeof (engine));
    p_engine->settings = p_settings;
    p_engine->resources = p_resources;
    p_engine->keep_running = 1;

    unsigned long milliseconds = 1000 / p_settings->MAX_TICKRATE;

    glog(LOG_FINE, "Engine: Sleep millis = %lu", milliseconds);

    p_engine->sleep.tv_sec = milliseconds / 1000;
    p_engine->sleep.tv_nsec = (milliseconds % 1000) * 1000000;

    glog(LOG_INFO, "Engine: Initialising summaries");
    statistics_init(&p_engine->stats);

    _engine_init_solo_commands(p_engine->command_proccess_func);
    _engine_init_game_prep_commands(p_engine->command_proccess_func);
    _engine_init_game_play_commands(p_engine->command_proccess_func);

    p_engine->p_netadapter = &p_engine->netadapter;
    p_engine->p_cmd_out = &p_engine->_cmd_out;

    cmd_queue_init(&p_engine->cmd_in_queue);

    return 0;
}

void _engine_handle_command(void *handler, const network_command cmd) {
    engine *p_engine = (engine *) handler;

    if (cmd_queue_is_full(&p_engine->cmd_in_queue)) {
        glog(LOG_ERROR, "Command queue is full");
        return;
    }

    cmd_queue_put(&p_engine->cmd_in_queue, cmd);
}

int _engine_process_command(engine *p, net_client *p_cli, network_command cmd) {
    int(* handle_action) ENGINE_HANDLE_FUNC_HEADER;
    str_scanner scanner;

    memset(p->p_cmd_out, 0, sizeof (network_command));

    if (cmd.type < 0 || cmd.type > NETWORK_COMMAND_TYPES_COUNT) {
        network_command_prepare(&p_cli->connection->_out_buffer, NCT_LEAD_DISCONNECT);
        snprintf(p_cli->connection->_out_buffer.data, NETWORK_COMMAND_DATA_LENGTH,
                g_loc.server_protection_illegal_cmd_type, cmd.type);
        engine_send_command(p, p_cli, &p_cli->connection->_out_buffer);
        return 1;
    }

    handle_action = p->command_proccess_func[cmd.type];
    if (handle_action == NULL) {
        glog(LOG_WARNING, "Engine: No handle action for %d", cmd.type);

        network_command_prepare(&p_cli->connection->_out_buffer, NCT_LEAD_DISCONNECT);
        snprintf(p_cli->connection->_out_buffer.data, NETWORK_COMMAND_DATA_LENGTH,
                g_loc.server_protection_unimplemented_cmd_type, cmd.type);
        engine_send_command(p, p_cli, &p_cli->connection->_out_buffer);

        return 2;
    }

    str_scanner_set(&scanner, cmd.data, cmd.length);

    if (handle_action(p, p_cli, &scanner)) {
        netadapter_handle_invallid_command(p->p_netadapter, p_cli, cmd);
        p->p_netadapter->stats->commands_received_invalid++;
        if (p_cli->connection->invalid_counter > p->p_netadapter->ALLOWED_INVALLID_MSG_COUNT) {
            return 3;
        }
    }

    return 0;
}

void _engine_process_queue(engine *p) {
    network_command cmd;

    while (!cmd_queue_is_empty(&p->cmd_in_queue)) {
        cmd = cmd_queue_get(&p->cmd_in_queue);
        net_client *p_cli = netadapter_get_client_by_aid(&p->netadapter, cmd.client_aid);

        int ret_val = _engine_process_command(p, p_cli, cmd);
        if (ret_val) {
            glog(LOG_FINE, "Engine: Closing connection on socket %02d, reason = %d", p_cli->connection->socket, ret_val);
            netadapter_close_connection_by_client(p->p_netadapter, p_cli);
        }
    }
}

void _engine_check_idle_client(engine *p, net_client *p_client, time_t now) {
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
    for (i = 0; i < p->resources->clients_length; i++) {
        p_cli = p->resources->clients + i;

        if (p_cli->connection != NULL) { // connection is open

        } else { // connection is not open
            if (p_cli->status != NET_CLIENT_STATUS_DISCONNECTED &&
                    p_cli->status != NET_CLIENT_STATUS_EMPTY) {
                glog(LOG_FINE, "Engine: Client %d found to be disconnected", i);
                p_cli->status = NET_CLIENT_STATUS_DISCONNECTED;
            }
        }

    }
}

void *engine_run(void *args) {
    engine *p_engine = (engine *) args;

    p_engine->netadapter.command_handler = p_engine;
    p_engine->netadapter.command_handle_func = &_engine_handle_command;

    p_engine->stats.run_start = clock();
    glog(LOG_INFO, "Engine: Starting");
    while (p_engine->keep_running) {
        _engine_process_queue(p_engine);
        _engine_check_active_clients(p_engine);

        p_engine->total_ticks++;
        if (p_engine->total_ticks > p_engine->settings->MAX_TICKRATE * 30) {

        }
        nanosleep(&p_engine->sleep, NULL);
    }

    p_engine->stats.run_end = clock();
    glog(LOG_INFO, "Engine: Finished");
    netadapter_shutdown(&p_engine->netadapter);
    return NULL;
}

int engine_count_clients(engine *p, unsigned char status) {
    int i, n = 0;
    net_client *p_client;
    for (i = 0; i < p->resources->clients_length; i++) {
        p_client = p->resources->clients + i;
        if (status == NET_CLIENT_STATUS_ANY ||
                (p_client->connection == NULL && (status == NET_CLIENT_STATUS_EMPTY)) ||
                (p_client->connection != NULL && (status == p_client->status))
                ) {
            n++;
        }
    }

    return n;
}

// netowrk command action related stuff

void engine_send_command(engine *p, net_client *p_cli, network_command *cmd) {
    netadapter_send_command(&p->netadapter, p_cli->connection, cmd);
}

void engine_bc_command(engine *p, game_room *p_gr, network_command *cmd) {
    netadapter_broadcast_command_p(&p->netadapter, p_gr->clients, p_gr->size, cmd);
}

game_room *engine_room_by_client(engine *p, net_client *p_cli) {
    if (p_cli->room_id == NETADAPTER_ITEM_EMPTY) {
        return NULL;
    }

    return p->resources->game_rooms + p_cli->room_id;
}

game_room *engine_find_empty_game_room(engine *p){
    int i;
    game_room *p_gr;
    
    for (i = 0; i < p->resources->game_rooms_length; i++) {
        p_gr = p->resources->game_rooms + i;
        if (p_gr->game_state == GAME_ROOM_STATE_IDLE) {
            return p_gr;
        }
    }
    
    return NULL;
}

void _engine_dump_room_to_client(engine *p, net_client *p_cli, game_room *p_gr){
    glog(LOG_WARNING, "Engine: Room dumping not implemented yet");
}

void engine_put_client_into_room(engine *p, net_client *p_cli, game_room *p_gr) {
    int i, clientRID;

    clientRID = game_room_put_client(p_gr, p_cli);
    if (clientRID == NETADAPTER_ITEM_EMPTY) {
        network_command_prepare(p->p_cmd_out, NCT_ROOMS_LEAVE);
        engine_send_command(p, p_cli, p->p_cmd_out);
        return;
    }
    
    p_cli->room_id = p_gr - p->resources->game_rooms;

    network_command_prepare(p->p_cmd_out, NCT_ROOMS_JOIN);
    network_command_append_byte(p->p_cmd_out, p_gr - p->resources->game_rooms);
    network_command_append_byte(p->p_cmd_out, clientRID); // local clientRID
    network_command_append_byte(p->p_cmd_out, p_gr->leaderClient); // leader clientRID

    engine_send_command(p, p_cli, p->p_cmd_out);

    network_command_prepare(p->p_cmd_out, NCT_ROOM_CLIENT_INFO);
    network_command_append_byte(p->p_cmd_out, clientRID);
    network_command_append_str(p->p_cmd_out, p_cli->name, strlen(p_cli->name));

    engine_bc_command(p, p_gr, p->p_cmd_out);
    
    _engine_dump_room_to_client(p, p_cli, p_gr);

    return;
}


