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

#define _CHUNK_SIZE 20
#define _MAX_TICKRATE 32

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
	
	return 0;
}

int main(int argc, char* argv[]) {
	define_consts();
	
	settings *p_settings = settings_process_arguments(argc - 1, argv + 1);
	*(int *)&p_settings->CHUNK_SIZE = _CHUNK_SIZE;
	*(unsigned int *)&p_settings->MAX_TICKRATE = _MAX_TICKRATE;
	
	networks *p_networks = networks_create(p_settings);
	engine *p_engine = engine_create(p_networks, p_settings);
	
	pthread_t thr_engine, thr_networks;
	
	pthread_create(&thr_engine, NULL, engine_run, p_engine);
	pthread_create(&thr_networks, NULL, networks_run, p_networks);
	
	p_engine->keep_running = 0;

	pthread_join(thr_engine, NULL);
	
	free(p_settings);
	
	return 0;
}
