
#include <stddef.h>

#include "network_command.h"

void network_command_prepare(network_command *p, short id, short type){
	p->message_id = id;
	p->cmd_type = type;
}