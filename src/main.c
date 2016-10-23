#include<stddef.h>
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
#define THR_ENGINE_INPUT 1
#define THR_NETW_SEND 2
#define THR_NETW_RECV 3

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

    //			TANK
    *(int *) &TANK_MAX_ENERGY = 24;
    *(int *) &TANK_MAX_HEALTH = 120;

    //			DIRECTIONS
    *(unsigned short *) &DIRECTION_0 = 0;
    *(unsigned short *) &DIRECTION_N = 1;
    *(unsigned short *) &DIRECTION_NE = 2;
    *(unsigned short *) &DIRECTION_E = 3;
    *(unsigned short *) &DIRECTION_SE = 4;
    *(unsigned short *) &DIRECTION_S = 5;
    *(unsigned short *) &DIRECTION_SW = 6;
    *(unsigned short *) &DIRECTION_W = 7;
    *(unsigned short *) &DIRECTION_NW = 8;

    DIRECTION_AXIS_X[DIRECTION_NW] = DIRECTION_AXIS_X[DIRECTION_W] = DIRECTION_AXIS_X[DIRECTION_SW] = -1;
    DIRECTION_AXIS_X[DIRECTION_N] = DIRECTION_AXIS_X[DIRECTION_0] = DIRECTION_AXIS_X[DIRECTION_S] = 0;
    DIRECTION_AXIS_X[DIRECTION_NE] = DIRECTION_AXIS_X[DIRECTION_E] = DIRECTION_AXIS_X[DIRECTION_SE] = 1;

    DIRECTION_AXIS_Y[DIRECTION_NW] = DIRECTION_AXIS_Y[DIRECTION_N] = DIRECTION_AXIS_Y[DIRECTION_NE] = -1;
    DIRECTION_AXIS_Y[DIRECTION_W] = DIRECTION_AXIS_Y[DIRECTION_0] = DIRECTION_AXIS_Y[DIRECTION_E] = 0;
    DIRECTION_AXIS_Y[DIRECTION_SW] = DIRECTION_AXIS_Y[DIRECTION_S] = DIRECTION_AXIS_Y[DIRECTION_SE] = 1;

    return 0;
}

void print_help() {
    printf("Include params pls\n");
}

int main(int argc, char* argv[]) {
    int ret_val;
    engine eng;
    pthread_t threads[4];

    define_consts();
    settings *p_settings = malloc(sizeof (settings));
    if (settings_process_arguments(p_settings, argc, argv)) {
        print_help();
        return MAIN_ERR_BAD_ARGS;
    }

    ret_val = engine_init(&eng, p_settings);
    switch (ret_val) {
        case ENGERR_NETWORK_INIT_FAILED:
            printf("Network interface couldn't be created, exitting. \n");
            return MAIN_ERR_NETWORK_FAILED;
    }



    pthread_create(threads + THR_ENGINE, NULL, engine_run, &eng);
    pthread_create(threads + THR_ENGINE_INPUT, NULL, engine_input_run, &eng);

    //	pthread_create(threads + THR_NETW_RECV, NULL, networks_receiver_run, p_networks);
    //	pthread_create(threads + THR_NETW_SEND, NULL, networks_sender_run, p_networks);


    pthread_join(threads[THR_ENGINE], NULL);
    pthread_join(threads[THR_ENGINE_INPUT], NULL);

    pthread_join(threads[THR_NETW_RECV], NULL);
    pthread_join(threads[THR_NETW_SEND], NULL);

    engine_delete(&eng);

    printf("Program exited gracefully.\n");
    return 0;
}
