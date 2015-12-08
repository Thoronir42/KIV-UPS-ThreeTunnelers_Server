#include "engine.h"


engine* create_engine(networks* nw){
	engine* tmp = malloc(sizeof engine);
	tmp->nw = nw;
	
	return tmp;
}

void delete_engine(engine* e){
	free(e->nw);
	free(e);
}
