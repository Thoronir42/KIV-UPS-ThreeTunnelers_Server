#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "network_command.h"
#include "../my_strings.h"
#include "net_client.h"
#include "../logger.h"

#define NETCMD_TYPE_UNKNOWN 1
#define NETCMD_DATA_TOO_LONG 2

void network_command_prepare(network_command *p, short type) {
    memset(p, 0, sizeof (network_command));
    p->type = type;
}

void network_command_strprep(network_command *p, short type, char* message) {
    network_command_prepare(p, type);
    network_command_set_data(p, message, strlen(message));
}

int network_command_set_data(network_command *p, const char *str, int length) {
    if (length > NETWORK_COMMAND_DATA_LENGTH) {
        return -1;
    }

    memcpy(p->data, str, length);
    p->length = length;
    return 0;
}

void network_command_append_str(network_command *p, const char *str, int length) {
    if (p->length + length > NETWORK_COMMAND_DATA_LENGTH) {
        glog(LOG_WARNING, "Attempted to append to command which would cause "
                "data buffer overflow");
        return;
    }

    memcpy(p->data + p->length, str, length);
    p->length += length;
}

void network_command_append_char(network_command *p, char val) {
    network_command_append_str(p, &val, sizeof(char));
}

void network_command_append_byte(network_command *p, my_byte val){
    char buf[2];
    write_hex_byte(buf, val);
    
    network_command_append_str(p, buf, sizeof(buf));
}

void network_command_append_short(network_command *p, short val){
    char buf[4];
    write_hex_short(buf, val);
    
    network_command_append_str(p, buf, sizeof(buf));
}

void network_command_append_int(network_command *p, int val){
    char buf[8];
    write_hex_int(buf, val);
    
    network_command_append_str(p, buf, sizeof(buf));
}

void network_command_append_long(network_command *p, long val){
    char buf[16];
    write_hex_long(buf, val);
    
    network_command_append_str(p, buf, sizeof(buf));
}

int network_command_from_string(network_command *dest, char *src, int length) {
    int scanned;
    memset(dest, 0, sizeof (network_command));
    scanned = 0;

    //dest->id = read_hex_byte(src + scanned);
    //scanned += 2;

    dest->type = read_hex_short(src + scanned);
    scanned += 4;

    if (dest->type == NCT_UNDEFINED) {
        return NETCMD_TYPE_UNKNOWN;
    }

    if (length - scanned > NETWORK_COMMAND_DATA_LENGTH) {
        return NETCMD_DATA_TOO_LONG;
        //length = NETWORK_COMMAND_DATA_LENGTH - scanned;
    }


    dest->length = length - scanned;
    memcpy(dest->data, src + scanned, dest->length);

    if (dest->data[dest->length - 1] == '\n') {
        dest->data[dest->length - 1] = '\0';
        dest->length--;
    }

    return 0;
}

int network_command_to_string(char *dest, network_command *src) {
    int a2write = 0;
    src->length = strlen(src->data);
    //write_hex_byte (dest + a2write, src->id);		a2write += 2;
    write_hex_short(dest + a2write, src->type);
    a2write += 4;
    //write_hex_short(dest + a2write, src->length);	a2write += 4;
    memcpy(dest + a2write, src->data, src->length);

    return a2write + src->length;
}

void network_command_print(const char *label, const network_command *command) {
    //printf("%8s: %03d:%05d:%05d - %s\n", label, command->id, command->type, command->length, command->data);
    printf("%8s: %04d - %s\n", label, (int) command->type, command->data);
}