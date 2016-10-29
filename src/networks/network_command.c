
#include <stddef.h>
#include <string.h>

#include "network_command.h"
#include "../my_strings.h"

void network_command_prepare(network_command *p, short id, short type) {
	p->id = id;
	p->type = type;

	p->client = NULL;
}

int network_command_from_string(network_command *dest, char *src) {
	int copy_length;
	memset(dest, 0, sizeof(network_command));
	
	dest->id = read_hex_byte(src + 0);      // 2
	dest->type = read_hex_short(src + 2);   // 6
	copy_length = dest->length = read_hex_short(src + 6); // 10
	if(copy_length > NETWORK_COMMAND_DATA_LENGTH){
		copy_length = NETWORK_COMMAND_DATA_LENGTH;
	}
	
	memcpy(dest->data, src + 10, copy_length);
	
	return 0;
}

int network_command_to_string(char *dest, network_command *src) {
	
}