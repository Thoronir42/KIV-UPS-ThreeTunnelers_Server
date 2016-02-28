#include "networks.h"

networks* networks_create(){
	networks *tmp = malloc(sizeof(networks));
	
	return tmp;
}

void networks_delete(networks* p_networks){
	free(p_networks);
}