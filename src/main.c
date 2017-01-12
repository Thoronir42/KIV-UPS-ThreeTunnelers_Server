#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "main.h"
#include "logger.h"
#include "settings.h"
#include "core/resources.h"

#include "core/engine.h"
#include "networks/netadapter.h"

#include "game/game_room.h"
#include "game/player.h"
#include "game/control_input.h"
#include "model/tank.h"
#include "model/direction.h"
#include "tests/test.h"
#include "localisation.h"

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

int main(int argc, char* argv[]) {
    int ret_val;
    settings s_settings;
    resources s_resources;

    ret_val = main_startup(argc, argv, &s_settings, &s_resources);
    if (ret_val) {
        return ret_val;
    }

    ret_val = main_run(&s_settings, &s_resources);


    resources_free(&s_resources);

    glog(LOG_INFO, "Main: Program exited gracefully.");
    printf("Thank you for using Three Tunnelers server software\n");
    
    return ret_val;
}

int main_startup(int argc, char *argv[], settings *p_settings, resources *p_resources) {
    int ret_val;
    time_t now;
    printf("Main: Processing arguments\n");
    ////
    ret_val = settings_process_arguments(p_settings, argc, argv);
    switch (ret_val) {
        case 0:
            now = time(NULL);
//            logger_init(now);
            logger_init_file(stdout);
            break;
        case ARGERR_RUN_TESTS:
            logger_init_file(stdout);
            run_tests();
            return MAIN_ERR_TEST_RUN;
        default:
            print_help(argv[0], ret_val);
            return MAIN_ERR_BAD_ARGS;
    }
    
    glog(LOG_INFO, "Main: Initialising enviroment");

    init_locale();
    directions_initialise();

    glog(LOG_INFO, "Main: Allocating resources");
    ret_val = resources_allocate(p_resources, p_settings->MAX_ROOMS,
            p_settings->MAX_PLAYERS_PER_ROOM, FD_SETSIZE);

    if (ret_val) {
        glog(LOG_ERROR, "Main: Could not allocate resources. Colde = %d", ret_val);
        return MAIN_ERR_RES_ALLOCATION_FAIL;
    }

    return 0;
}

void _main_run_threads(engine *p_engine) {
    pthread_t threads[3];

    glog(LOG_INFO, "Run: Starting threads");
    pthread_create(threads + THR_ENGINE, NULL, engine_run, p_engine);
    pthread_create(threads + THR_NETADAPTER, NULL, netadapter_thread_select, &p_engine->netadapter);
    pthread_create(threads + THR_ENGINE_CLI, NULL, engine_cli_run, p_engine);

    glog(LOG_INFO, "Run: Waiting for threads to finish");
    pthread_join(threads[THR_ENGINE], NULL);
    pthread_join(threads[THR_NETADAPTER], NULL);
    pthread_join(threads[THR_ENGINE_CLI], NULL);

    glog(LOG_INFO, "Run: Threads have ended");
    if (p_engine->settings->show_statistics) {
        printf("Printing runtime statistics : \n");
        statistics_print(&p_engine->stats);
    }
}

int main_run(settings *p_settings, resources *p_resources) {
    int ret_val;
    engine s_engine;
    engine *p_engine = &s_engine;

    ////
    glog(LOG_INFO, "Main: Initialising engine and netadapter");
    ////
    engine_init(p_engine, p_settings, p_resources);
    ret_val = netadapter_init(&p_engine->netadapter, p_settings->port, &p_engine->stats,
            p_resources->clients, p_resources->clients_length,
            p_resources->connections, p_resources->con_to_cli, p_resources->connections_length);
    if (ret_val) {
        glog(LOG_WARNING, "Network interface couldn't be created, exitting.");
        ret_val = MAIN_ERR_NETWORK_FAILED;
    } else {
        _main_run_threads(p_engine);

        ret_val = EXIT_SUCCESS;
    }
}
