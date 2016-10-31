#include <stdio.h>
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
	dest->type = read_hex_short(src + 1);   // 6 //// pozor: misto +2 je tu +1, kdo vi proc?
	copy_length = dest->length = read_hex_short(src + 6); // 10
	if(copy_length > NETWORK_COMMAND_DATA_LENGTH){
		copy_length = NETWORK_COMMAND_DATA_LENGTH;
	}
	
	memcpy(dest->data, src + 10, copy_length);
	
	return 0;
}

int network_command_to_string(char *dest, network_command *src) {
	int a2write = 0;
	src->length = strlen(src->data);
	write_hex_byte (dest + a2write, src->id);		a2write += 2;	// 2
	write_hex_short(dest + a2write, src->type);		a2write += 4;	// 6
	write_hex_short(dest + a2write, src->length);	a2write += 4;	// 10
	memcpy(dest + a2write, src->data, src->length);
	
	return a2write + src->length;
}

void network_command_print(const char *label, const network_command *command){
	printf("%8s: %03d:%05d:%05d - %s\n", label, command->id, command->type, command->length, command->data);
}