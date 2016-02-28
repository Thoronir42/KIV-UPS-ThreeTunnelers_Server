#ifndef ENGINE_H
#define ENGINE_H

#include "networks.h"

typedef struct engine{
	networks* nw;
	
} engine;

engine *engine_create();

void engine_delete(engine *e);

void *engine_run(){
	
}



#endif
