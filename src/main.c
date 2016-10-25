#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "settings.h"

#include "core/engine.h"

#include "game/game_room.h"
#include "game/player.h"
#include "game/input.h"
#include "model/tank.h"
#include "model/direction.h"

#define MAIN_ERR_BAD_ARGS 1
#define MAIN_ERR_NETWORK_FAILED 2

#define THR_ENGINE 0
#define THR_NETADAPTER 1

int define_consts() {
    //			GAME_ROOM
    *(int *) &GAME_ROOM_MAX_PLAYERS = 4;

    //			PLAYER
    *(int *) &PLAYER_NONE = -1;
    *(int *) &PLAYER_SERVER = 0;
    *(int *) &PLAYER_FIRST_USABLE = 40;

    //			INPUT
    *(int *) &INPUT_MASK_UP = 0x01;
    *(int *) &INPUT_MASK_DOWN = 0x02;
    *(int *) &INPUT_MASK_LEFT = 0x04;
    *(int *) &INPUT_MASK_RIGHT = 0x08;
    *(int *) &INPUT_MASK_SHOOT = 0x10;
    

    DIRECTION_AXIS_X[DIRECTION_NW] = DIRECTION_AXIS_X[DIRECTION_W] = DIRECTION_AXIS_X[DIRECTION_SW] = -1;
    DIRECTION_AXIS_X[DIRECTION_N] = DIRECTION_AXIS_X[DIRECTION_0] = DIRECTION_AXIS_X[DIRECTION_S] = 0;
    DIRECTION_AXIS_X[DIRECTION_NE] = DIRECTION_AXIS_X[DIRECTION_E] = DIRECTION_AXIS_X[DIRECTION_SE] = 1;

    DIRECTION_AXIS_Y[DIRECTION_NW] = DIRECTION_AXIS_Y[DIRECTION_N] = DIRECTION_AXIS_Y[DIRECTION_NE] = -1;
    DIRECTION_AXIS_Y[DIRECTION_W] = DIRECTION_AXIS_Y[DIRECTION_0] = DIRECTION_AXIS_Y[DIRECTION_E] = 0;
    DIRECTION_AXIS_Y[DIRECTION_SW] = DIRECTION_AXIS_Y[DIRECTION_S] = DIRECTION_AXIS_Y[DIRECTION_SE] = 1;

    return 0;
}

void print_help(char *file, int err) {
    switch(err){
        case ARGERR_NOT_ENOUGH_ARGUMENTS:
            printf("Not enough arguments \n");
            break;
    }
    
    printf("Usage: %s port rooms\n", file);
    printf("  port      port number for binding in range of 0 - 65535\n");
    printf("  rooms     maximum concurent game rooms, recommended amount is 10\n");
}

void dump_args(int argc, char *argv[]){
    int i;
    for(i = 0; i < argc; i++){
        printf("%d: %s\n", i, argv[i]);
    }
}

int main(int argc, char* argv[]) {
    int ret_val;
    engine eng;
    pthread_t threads[2];

    printf("Main: Defining constants\n");
    define_consts();
    
    printf("Main: Processing arguments\n");
    settings *p_settings = malloc(sizeof (settings));
    ret_val = settings_process_arguments(p_settings, argc, argv);
    if (ret_val) {
        print_help(argv[0], ret_val);
        return MAIN_ERR_BAD_ARGS;
    }

    printf("Main: Initialising engine\n");
    ret_val = engine_init(&eng, p_settings);
    switch (ret_val) {
        case ENGERR_NETWORK_INIT_FAILED:
            printf("Network interface couldn't be created, exitting. \n");
            return MAIN_ERR_NETWORK_FAILED;
    }


    printf("Main: Starting threads\n");
    pthread_create(threads + THR_ENGINE, NULL, engine_run, &eng);

    //	pthread_create(threads + THR_NETW_RECV, NULL, networks_receiver_run, p_networks);
    //	pthread_create(threads + THR_NETW_SEND, NULL, networks_sender_run, p_networks);

    printf("Main: Waiting for threads to finish\n");
    pthread_join(threads[THR_ENGINE], NULL);

//    pthread_join(threads[THR_NETW_RECV], NULL);
//    pthread_join(threads[THR_NETW_SEND], NULL);
    
    if (eng.settings->show_summaries) {
        printf("Main: Engine has ended, printing sumamry: \n");
        summary_print(&eng.summary);
    }

    printf("Main: Program exited gracefully.\n");
    return EXIT_SUCCESS;
}
