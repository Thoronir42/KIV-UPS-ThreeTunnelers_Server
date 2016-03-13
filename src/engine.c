#include <stdlib.h>
#include <stddef.h>
#include <time.h>

#include "engine.h"

#include "networks/networks.h"

#include "settings.h"
#include "run_summary.h"

#define CLOCK CLOCK_MONOTONIC


engine* engine_create(networks *p_networks, settings *p_settings){
	engine* tmp = malloc(sizeof(engine));
	tmp->p_networks = p_networks;
	tmp->p_settings = p_settings;
	tmp->p_summary = run_summary_create();
	
	tmp->total_ticks = 0;
	tmp->usleep_length = 1000000 / p_settings->MAX_TICKRATE;
	
	return tmp;
}

void engine_delete(engine *p_engine){
	networks_delete(p_engine->p_networks);
	run_summary_delete(p_engine->p_summary);
	free(p_engine->p_settings);
	free(p_engine);
}

void *engine_run(engine *p_engine){
	p_engine->keep_running = 1;
	p_engine->p_summary->run_start = clock();
	while(p_engine->keep_running){
		if(!p_engine->total_ticks % p_engine->p_settings->MAX_TICKRATE )
		printf("Engine runs * %d.\n", p_engine->total_ticks / p_engine->p_settings->MAX_TICKRATE);
		if(++p_engine->total_ticks > p_engine->p_settings->MAX_TICKRATE * 2){
			p_engine->keep_running = 0;
		}
		
		usleep(p_engine->usleep_length);
	}
	p_engine->p_summary->run_end = clock();
	if(p_engine->p_settings->show_summaries){
		printf("Engine has ended, printing sumamry: \n");
		run_summary_print(p_engine->p_summary);
	}
	
	networks_shutdown(p_engine->p_networks);
	
	return NULL;
}