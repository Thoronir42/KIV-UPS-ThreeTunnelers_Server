#ifndef NETWORK_COMMANDS_H
#define NETWORK_COMMANDS_H

#include "../my_strings.h"

#define NETWORK_COMMAND_HEADER_SIZE 4
#define NETWORK_COMMAND_DATA_LENGTH 512

#define NETWORK_COMMAND_GAME_ROOM_LENGTH 12
#define NETWORK_COMMAND_BLOCK_CHANGE_LENGTH 12
#define NETWORK_COMMAND_TYPES_COUNT 256

typedef enum network_command_type
{
    NCT_UNDEFINED = 0x00,
    NCT_LEAD_INTRODUCE = 0x01,
    NCT_LEAD_DISCONNECT = 0x02,
    NCT_CLIENT_SET_NAME = 0x03,
    NCT_LEAD_MARCO = 0x04,
    NCT_LEAD_POLO = 0x05,
    
    NCT_ROOMS_LIST = 0x10,
    NCT_ROOMS_CREATE = 0x12,
    NCT_ROOMS_JOIN = 0x13,
    NCT_ROOMS_LEAVE = 0x14,

    NCT_MSG_PLAIN = 0x20,
    NCT_MSG_SYSTEM = 0x21,
    NCT_MSG_RCON = 0x22,

    NCT_ROOM_SYNC_STATE = 0x40,
    NCT_ROOM_READY_STATE = 0x41,

    NCT_ROOM_CLIENT_INFO = 0x42,
    NCT_ROOM_CLIENT_STATUS = 0x43,
    NCT_ROOM_CLIENT_REMOVE = 0x44,
    NCT_ROOM_SET_LEADER = 0x45,

    NCT_ROOM_PLAYER_ATTACH = 0x50,
    NCT_ROOM_PLAYER_DETACH = 0x51,
    NCT_ROOM_PLAYER_MOVE = 0x52,
    NCT_ROOM_PLAYER_SET_COLOR = 0x53,

    NCT_MAP_SPECIFICATION = 0x60,
    NCT_MAP_BASES = 0x61,
    NCT_MAP_CHUNK_DATA = 0x62,
    NCT_MAP_BLOCK_CHANGES = 0x63,

    NCT_GAME_CONTROLS_SET = 0x70,
    NCT_GAME_TANK_INFO = 0x74,
    NCT_GAME_PROJ_ADD = 0x7A,
    NCT_GAME_PROJ_REM = 0x7B,
} network_command_type;

typedef struct network_command
{
    network_command_type type;
    short length;
    int origin_socket;

    char data[NETWORK_COMMAND_DATA_LENGTH + 1];

} network_command;

void network_command_prepare(network_command *p, network_command_type type);

void network_command_strprep(network_command *p, network_command_type type, char* message);

int network_command_set_data(network_command *p, const char *str, int length);


void network_command_append_string(network_command *p, const char *str, int length);
void network_command_append_str(network_command *p, const char *str);

void network_command_append_char(network_command *p, char val);

void network_command_append_byte(network_command *p, my_byte val);
void network_command_append_short(network_command *p, short val);
void network_command_append_int(network_command *p, int val);
//void network_command_append_long(network_command *p, long val);
void network_command_append_number(network_command *p, int n);

int network_command_has_room_for(network_command *p, int length);

/**
 * 
 * @param dest
 * @param src
 * @param length
 * @return status of parsing: 0 if ok, 1 if data was too long
 */
int network_command_from_string(network_command *dest, char *src, int length);

int network_command_to_string(char *dest, network_command *src);

void network_command_print(const char *label, const network_command *command);

#endif /* NETWORK_COMMANDS_H */

