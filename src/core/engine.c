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

void _engine_check_active_clients(engine *p) {
    // todo: implement
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

int engine_client_rid_by_client(engine *p, net_client *p_cli) {
    int i;
    game_room *p_gr = engine_room_by_client(p, p_cli);
    if (p_gr == NULL) {
        return NETADAPTER_ITEM_EMPTY;
    }

    for (i = 0; i < GAME_ROOM_MAX_PLAYERS; i++) {
        if (p_gr->clients[i] == p_cli) {
            return i + 1;
        }
    }

    return NETADAPTER_ITEM_EMPTY;

}

game_room *engine_room_by_client(engine *p, net_client *p_cli) {
    if (p_cli->room_id == NETADAPTER_ITEM_EMPTY) {
        return NULL;
    }

    return p->resources->game_rooms + p_cli->room_id;
}
