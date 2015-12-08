#ifndef ENGINE_H
#define ENGINE_H

#include "networks.h"

typedef struct engine{
	networks* nw;
	
} engine;

engine* create_engine();

void delete_engine(engine* e);





#endif
