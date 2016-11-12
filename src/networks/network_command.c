#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "network_command.h"
#include "../my_strings.h"
#include "net_client.h"

void network_command_prepare(network_command *p, short id, short type) {
    p->id = id;
    p->type = type;

    p->client_aid = NET_CLIENT_ID_EMPTY;
}

int network_command_from_string(network_command *dest, char *src, int length) {
    int scanned;
    memset(dest, 0, sizeof (network_command));
    scanned = 0;

    //dest->id = read_hex_byte(src + scanned);
    //scanned += 2;
    
    dest->type = read_hex_short(src + scanned);
    scanned += 4;
    
    if (length - scanned > NETWORK_COMMAND_DATA_LENGTH) {
        length = NETWORK_COMMAND_DATA_LENGTH - scanned;
    }

    dest->_length = length - scanned;
    memcpy(dest->data, src + scanned, dest->_length);
    

    return 0;
}

int network_command_to_string(char *dest, network_command *src) {
    int a2write = 0;
    src->_length = strlen(src->data);
    //write_hex_byte (dest + a2write, src->id);		a2write += 2;
    write_hex_short(dest + a2write, src->type);
    a2write += 4;
    //write_hex_short(dest + a2write, src->length);	a2write += 4;
    memcpy(dest + a2write, src->data, src->_length);

    return a2write + src->_length;
}

void network_command_print(const char *label, const network_command *command) {
    //printf("%8s: %03d:%05d:%05d - %s\n", label, command->id, command->type, command->length, command->data);
    printf("%8s: %04d - %s\n", label, command->type, command->data);
}