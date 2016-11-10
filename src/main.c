#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "main.h"
#include "settings.h"
#include "data/resources.h"

#include "core/engine.h"
#include "networks/netadapter.h"

#include "game/game_room.h"
#include "game/player.h"
#include "game/control_input.h"
#include "model/tank.h"
#include "model/direction.h"
#include "tests/test.h"

int define_consts() {
    // -1 on X (West)
    DIRECTION_AXIS_X[DIRECTION_NW] = DIRECTION_AXIS_X[DIRECTION_W] = DIRECTION_AXIS_X[DIRECTION_SW] = -1;
    //  0 on X (neutral)
    DIRECTION_AXIS_X[DIRECTION_N] = DIRECTION_AXIS_X[DIRECTION_0] = DIRECTION_AXIS_X[DIRECTION_S] = 0;
    // +1 on X (East)
    DIRECTION_AXIS_X[DIRECTION_NE] = DIRECTION_AXIS_X[DIRECTION_E] = DIRECTION_AXIS_X[DIRECTION_SE] = +1;

    // -1 on Y (North)
    DIRECTION_AXIS_Y[DIRECTION_NW] = DIRECTION_AXIS_Y[DIRECTION_N] = DIRECTION_AXIS_Y[DIRECTION_NE] = -1;
    //  0 on Y (neutral)
    DIRECTION_AXIS_Y[DIRECTION_W] = DIRECTION_AXIS_Y[DIRECTION_0] = DIRECTION_AXIS_Y[DIRECTION_E] = 0;
    // +1 on Y (South)
    DIRECTION_AXIS_Y[DIRECTION_SW] = DIRECTION_AXIS_Y[DIRECTION_S] = DIRECTION_AXIS_Y[DIRECTION_SE] = +1;

    return 0;
}

int main(int argc, char* argv[]) {
    int ret_val;
    pthread_t threads[2];

    settings s_settings;
    resources s_resources;
    engine s_engine;

    settings *p_settings = &s_settings;
    resources *p_resources = &s_resources;
    engine *p_engine = &s_engine;

    define_consts();

    printf("Main: Processing arguments\n");

    ret_val = settings_process_arguments(p_settings, argc, argv);
    if (ret_val == ARGERR_RUN_TESTS) {
        run_tests();
        return 0;
    }

    if (ret_val) {
        print_help(argv[0], ret_val);
        return MAIN_ERR_BAD_ARGS;
    }

    printf("Main: Allocating resources\n");
    resources_allocate(p_resources, p_settings->MAX_ROOMS,
            p_settings->MAX_PLAYERS_PER_ROOM, NETADAPTER_FD_RESERVE);

    printf("Main: Initialising engine and netadapter\n");
    engine_init(p_engine, p_settings, p_resources);
    if (netadapter_init(&p_engine->netadapter, p_settings->port,
            p_resources->clients, p_resources->clients_size, p_resources->fd_to_client)) {
        printf("Network interface couldn't be created, exitting. \n");
        ret_val = MAIN_ERR_NETWORK_FAILED;
    } else {
        main_run_threads(threads, p_engine);

        ret_val = EXIT_SUCCESS;
    }

    printf("Main: Freeing resources.\n");
    resources_free(&s_resources);

    printf("Main: Program exited gracefully.\n");
    return ret_val;
}

void print_help(const char *file, int err) {
    switch (err) {
        case ARGERR_NOT_ENOUGH_ARGUMENTS:
            printf("Not enough arguments \n");
            break;
    }

    printf("Usage: %s port rooms\n", file);
    printf("  port      port number for binding in range of 0 - 65535\n");
    printf("  rooms     maximum concurent game rooms, recommended amount is 10\n");
}

void main_run_threads(pthread_t *threads, engine *p_engine) {
    printf("Run: Starting threads\n");
    pthread_create(threads + THR_ENGINE, NULL, engine_run, p_engine);
    pthread_create(threads + THR_ENGINE_CLI, NULL, engine_cli_run, p_engine);
    //    pthread_create(threads + THR_NETADAPTER, NULL, netadapter_thread_select, &p_engine->netadapter);

    printf("Run: Waiting for threads to finish\n");
    pthread_join(threads[THR_ENGINE], NULL);
    pthread_join(threads[THR_ENGINE_CLI], NULL);
    //pthread_join(threads[THR_NETADAPTER], NULL);

    printf("Run: Engine has ended");
    if (p_engine->settings->show_summaries) {
        printf(", printing sumamry: \n");
        summary_print(&p_engine->summary);
    }

    printf("\n");
}
