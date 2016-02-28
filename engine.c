#include "engine.h"


engine* engine_create(networks *nw){
	engine* tmp = malloc(sizeof engine);
	tmp->nw = nw;
	
	return tmp;
}

void engine_delete(engine *p_engine){
	networks_delete(p_engine->nw);
	free(p_engine);
}

void *engine_run(){
	
}