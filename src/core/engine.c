#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "engine.h"
#include "resources.h"

#include "../networks/netadapter.h"

#include "../logger.h"
#include "../settings.h"
#include "../statistics.h"
#include "../game/control_input.h"

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
    p_engine->p_cmd_out = &p_engine->cmd_out;

    return 0;
}

int _engine_handle_command(void *handler, const network_command cmd) {
    engine *p_engine = (engine *) handler;
    net_client *p_client = netadapter_get_client_by_aid(&p_engine->netadapter, cmd.client_aid);
    int(* handle_action) ENGINE_HANDLE_FUNC_HEADER;
    str_scanner scanner;

    memset(p_engine->p_cmd_out, 0, sizeof (network_command));

    if (cmd.type == NCT_UNDEFINED) {
        return 1;
    }

    handle_action = p_engine->command_proccess_func[cmd.type];
    if (handle_action == NULL) {
        glog(LOG_WARNING, "Engine: No handle action for %d", cmd.type);
        return 2;
    }

    str_scanner_set(&scanner, cmd.data, cmd.length);

    return handle_action(p_engine, p_client, &scanner);
}

void *engine_run(void *args) {
    engine *p_engine = (engine *) args;

    p_engine->netadapter.command_handler = p_engine;
    p_engine->netadapter.command_handle_func = &_engine_handle_command;

    p_engine->stats.run_start = clock();
    glog(LOG_INFO, "Engine: Starting");
    while (p_engine->keep_running) {
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
