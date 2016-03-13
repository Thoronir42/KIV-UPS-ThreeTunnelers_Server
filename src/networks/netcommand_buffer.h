#ifndef NETCOMMAND_BUFFER_H
#define	NETCOMMAND_BUFFER_H

#include "network_command.h"

typedef struct netcommand_buffer{
	network_command *content;
	
	int size;
	int current;
} netcommand_buffer;


netcommand_buffer *netcommand_buffer_create(int size);

void netcommand_buffer_delete(netcommand_buffer *p);

void netcommand_buffer_put(netcommand_buffer *p, network_command *ncmd);

network_command *netcommand_buffer_get(netcommand_buffer *p, network_command *ncmd);

#endif	/* NETCOMMAND_BUFFER_H */

