
#include <string.h>

#include "netcommand_buffer.h"

netcommand_buffer *netcommand_buffer_create(int size){
	netcommand_buffer *tmp = malloc(sizeof(netcommand_buffer));
	tmp->size = size;
	tmp->current = 0;
	
	tmp->content = malloc(sizeof(netcommand_buffer) * size);
	
	return tmp;
}

void netcommand_buffer_delete(netcommand_buffer *p){
	free(p->content);
	
	free(p);
}

void netcommand_buffer_put(netcommand_buffer *p, network_command *ncmd){
	p->content[0] = *ncmd;
}

network_command *netcommand_buffer_get(netcommand_buffer *p, network_command *ncmd){
	*ncmd = p->content[0];
}