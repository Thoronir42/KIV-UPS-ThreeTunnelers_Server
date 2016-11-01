#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "settings.h"

#include "core/engine.h"
#include "networks/netadapter.h"

#include "game/game_room.h"
#include "game/player.h"
#include "game/control_input.h"
#include "model/tank.h"
#include "model/direction.h"

#define MAIN_ERR_BAD_ARGS 1
#define MAIN_ERR_NETWORK_FAILED 2

#define THR_ENGINE 0
#define THR_NETADAPTER 1

int define_consts() {
	//			GAME_ROOM
	*(int *) &GAME_ROOM_MAX_PLAYERS = 4;


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

void print_help(char *file, int err) {
	switch (err) {
		case ARGERR_NOT_ENOUGH_ARGUMENTS:
			printf("Not enough arguments \n");
			break;
	}

	printf("Usage: %s port rooms\n", file);
	printf("  port      port number for binding in range of 0 - 65535\n");
	printf("  rooms     maximum concurent game rooms, recommended amount is 10\n");
}

void dump_args(int argc, char *argv[]) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%d: %s\n", i, argv[i]);
	}
}

int main(int argc, char* argv[]) {
	int ret_val;
	engine eng;
	settings s_settings;
	pthread_t threads[2];

	settings *p_settings = &s_settings;
	
	printf("Main: Defining constants\n");
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

	printf("Main: Initialising engine\n");
	ret_val = engine_init(&eng, p_settings);
	switch (ret_val) {
		case ENGERR_NETWORK_INIT_FAILED:
			printf("Network interface couldn't be created, exitting. \n");
			return MAIN_ERR_NETWORK_FAILED;
	}


	printf("Main: Starting threads\n");
	//    pthread_create(threads + THR_ENGINE, NULL, engine_run, &eng);
	pthread_create(threads + THR_NETADAPTER, NULL, netadapter_thread_select, &eng.netadapter);

	printf("Main: Waiting for threads to finish\n");
	//    pthread_join(threads[THR_ENGINE], NULL);
	pthread_join(threads[THR_NETADAPTER], NULL);

	//    pthread_join(threads[THR_NETW_RECV], NULL);
	//    pthread_join(threads[THR_NETW_SEND], NULL);

	if (eng.settings->show_summaries) {
		printf("Main: Engine has ended, printing sumamry: \n");
		summary_print(&eng.summary);
	}

	printf("Main: Program exited gracefully.\n");
	return EXIT_SUCCESS;
}
