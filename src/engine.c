#include <stdlib.h>
#include <stddef.h>

#include "engine.h"
#include "settings.h"


engine* engine_create(networks *p_networks, settings *p_settings){
	engine* tmp = malloc(sizeof(engine));
	tmp->p_networks = p_networks;
	tmp->p_settings = p_settings;
	
	return tmp;
}

void engine_delete(engine *p_engine){
	networks_delete(p_engine->p_networks);
	free(p_engine);
}

void *engine_run(engine *p_engine){
	p_engine->keep_running = 1;
	while(p_engine->keep_running){
		printf("Engine runs.\n");
		p_engine->keep_running = 0;
		
	}
	printf("Engine has run.\n");
	
	return NULL;
}