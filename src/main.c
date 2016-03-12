#include<stddef.h>
#include <stdlib.h>
#include <pthread.h>

#include "settings.h"
#include "networks/networks.h"

#include "engine.h"

#include "game/game_room.h"
#include "game/player.h"
#include "game/input.h"
#include "game/tank.h"
#include "game/tunneler_map_block.h"
#include "game/direction.h"

#define _CHUNK_SIZE 20
#define _MAX_TICKRATE 32

#define RUN_ERROR_NO_ARGS 1
#define RUN_ERROR_NETWORK_FAILED 1

int define_consts(){
	//			GAME_ROOM
	*(int *)&GAME_ROOM_MAX_PLAYERS = 1;
	*(int *)&GAME_ROOM_PASS_MAX_LENGTH = 32;
	*(int *)&GAME_ROOM_STATE_LOBBY = 1;
	*(int *)&GAME_ROOM_STATE_RUNNING = 2;
	*(int *)&GAME_ROOM_STATE_DONE = 3;
	
	//			PLAYER
	*(int *)&PLAYER_NONE = 0;
	*(int *)&PLAYER_SERVER = 1;
	*(int *)&PLAYER_FIRST_USABLE = 40;
	
	//			INPUT
	*(int *)&INPUT_MASK_UP = 0x01;
	*(int *)&INPUT_MASK_DOWN = 0x02;
	*(int *)&INPUT_MASK_LEFT = 0x04;
	*(int *)&INPUT_MASK_RIGHT = 0x08;
	*(int *)&INPUT_MASK_SHOOT = 0x10;
	
	//			TANK
	*(int *)&TANK_MAX_ENERGY = 24;
	*(int *)&TANK_MAX_HEALTH = 120;
	
	//			MAP_BLOCK
	*(int *)&MAP_BLOCK_EMPTY = 1;
	*(int *)&MAP_BLOCK_EARTH = 2;
	*(int *)&MAP_BLOCK_ROCK = 3;
	*(int *)&MAP_BLOCK_WALL = 4;
	
	//			DIRECTIONS
	*(unsigned short *)&DIRECTION_0 = 0;
	*(unsigned short *)&DIRECTION_N = 1;
	*(unsigned short *)&DIRECTION_NE = 2;
	*(unsigned short *)&DIRECTION_E = 3;
	*(unsigned short *)&DIRECTION_SE = 4;
	*(unsigned short *)&DIRECTION_S = 5;
	*(unsigned short *)&DIRECTION_SW = 6;
	*(unsigned short *)&DIRECTION_W = 7;
	*(unsigned short *)&DIRECTION_NW = 8;
	
	DIRECTION_AXIS_X[DIRECTION_NW] = DIRECTION_AXIS_X[DIRECTION_W] = DIRECTION_AXIS_X[DIRECTION_SW] =-1;
	DIRECTION_AXIS_X[DIRECTION_N]  = DIRECTION_AXIS_X[DIRECTION_0] = DIRECTION_AXIS_X[DIRECTION_S]  = 0;
	DIRECTION_AXIS_X[DIRECTION_NE] = DIRECTION_AXIS_X[DIRECTION_E] = DIRECTION_AXIS_X[DIRECTION_SE] = 1;
	
	DIRECTION_AXIS_Y[DIRECTION_NW] = DIRECTION_AXIS_Y[DIRECTION_N] = DIRECTION_AXIS_Y[DIRECTION_NE] =-1;
	DIRECTION_AXIS_Y[DIRECTION_W]  = DIRECTION_AXIS_Y[DIRECTION_0] = DIRECTION_AXIS_Y[DIRECTION_E]  = 0;
	DIRECTION_AXIS_Y[DIRECTION_SW] = DIRECTION_AXIS_Y[DIRECTION_S] = DIRECTION_AXIS_Y[DIRECTION_SE] = 1;
	
	return 0;
}

void print_help(){
	printf("Include params pls\n");
}

int main(int argc, char* argv[]) {
	define_consts();
	settings *p_settings = malloc(sizeof(settings));
	if(settings_process_arguments(p_settings, argc, argv)){
		print_help();
	}
	
	*(int *)(&p_settings->CHUNK_SIZE) = _CHUNK_SIZE;
	*(unsigned int *)&p_settings->MAX_TICKRATE = _MAX_TICKRATE;
	
	networks *p_networks = networks_create(p_settings);
	if(!p_networks){
		printf("Network interface couldn't be created, exitting. \n");
		return RUN_ERROR_NETWORK_FAILED;
	}
	
	
	engine *p_engine = engine_create(p_networks, p_settings);
	
	pthread_t thr_engine, thr_networks;
	
	pthread_create(&thr_engine, NULL, engine_run, p_engine);
	
	pthread_create(&thr_networks, NULL, networks_receiver_run, p_networks);
	pthread_create(&thr_networks, NULL, networks_sender_run, p_networks);
	
	p_engine->keep_running = 0;

	pthread_join(thr_engine, NULL);
	pthread_join(thr_networks, NULL);
	
	engine_delete(p_engine);
	
	printf("Program exited gracefully.\n");
	return 0;
}
