#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "network_command.h"
#include "../my_strings.h"
#include "net_client.h"

#define NETCMD_TYPE_UNKNOWN 1
#define NETCMD_DATA_TOO_LONG 2

void network_command_prepare(network_command *p, short type) {
    memset(p, 0, sizeof (network_command));
    p->type = type;
}

void network_command_strprep(network_command *p, short type, char* message) {
    network_command_prepare(p, type);
    memcpy(p->data, message, strlen(message));
}

int network_command_set_data(network_command *p, const char *str, int length) {
    if(length > NETWORK_COMMAND_DATA_LENGTH){
        return -1;
    }
    
    memcpy(p->data, str, length);
    
    return 0;
}

int network_command_from_string(network_command *dest, char *src, int length) {
    int scanned;
    memset(dest, 0, sizeof (network_command));
    scanned = 0;

    //dest->id = read_hex_byte(src + scanned);
    //scanned += 2;

    dest->type = read_hex_short(src + scanned);
    scanned += 4;
    
    if(dest->type == NCT_UNDEFINED){
        return NETCMD_TYPE_UNKNOWN;
    }

    if (length - scanned > NETWORK_COMMAND_DATA_LENGTH) {
        return NETCMD_DATA_TOO_LONG;
        //length = NETWORK_COMMAND_DATA_LENGTH - scanned;
    }


    dest->_length = length - scanned;
    memcpy(dest->data, src + scanned, dest->_length);

    if (dest->data[dest->_length - 1] == '\n') {
        dest->data[dest->_length - 1] = '\0';
        dest->_length--;
    }

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
    printf("%8s: %04d - %s\n", label, (int) command->type, command->data);
}