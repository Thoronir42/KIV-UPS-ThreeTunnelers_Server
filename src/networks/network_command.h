#ifndef NETWORK_COMMANDS_H
#define NETWORK_COMMANDS_H

#include "../my_strings.h"

#define NETWORK_COMMAND_HEADER_SIZE 4
#define NETWORK_COMMAND_DATA_LENGTH 512

#define NETWORK_COMMAND_GAME_ROOM_LENGTH 12

// NCT as Network Command Type
#define NETWORK_COMMAND_TYPES_COUNT 256
#define NCT_UNDEFINED 0

#define NCT_LEAD_INTRODUCE 1
#define NCT_LEAD_DISCONNECT 2
#define NCT_LEAD_MARCO 3
#define NCT_LEAD_POLO 4
#define NCT_LEAD_BAD_FORMAT 5


#define NCT_CLIENT_SET_NAME 10

#define NCT_ROOMS_LIST 14
#define NCT_ROOMS_CREATE 16
#define NCT_ROOMS_JOIN 17
#define NCT_ROOMS_LEAVE 18

#define NCT_MSG_PLAIN 40
#define NCT_MSG_RCON 42

#define NCT_ROOM_SYNC_PHASE 60
#define NCT_ROOM_READY_STATE 61


#define NCT_ROOM_CLIENT_INFO 65
#define NCT_ROOM_CLIENT_LATENCY 66
#define NCT_ROOM_CLIENT_REMOVE 67
#define NCT_ROOM_SET_LEADER 68

#define NCT_ROOM_PLAYER_ATTACH 80
#define NCT_ROOM_PLAYER_DETACH 81
#define NCT_ROOM_PLAYER_MOVE 82
#define NCT_ROOM_PLAYER_SET_COLOR 83

#define NCT_MAP_SPECIFICATION 90
#define NCT_MAP_CHUNK_DATA 91
#define NCT_MAP_BLOCK_CHANGES 92

#define NCT_GAME_CONTROLS_SET 120
#define NCT_GAME_TANK_INFO 130
#define NCT_GAME_PROJ_ADD 140
#define NCT_GAME_PROJ_REM 141

typedef struct network_command
{
    short type;
    short length;
    int origin_socket;

    char data[NETWORK_COMMAND_DATA_LENGTH + 1];

} network_command;

void network_command_prepare(network_command *p, short type);

void network_command_strprep(network_command *p, short type, char* message);

int network_command_set_data(network_command *p, const char *str, int length);


void network_command_append_string(network_command *p, const char *str, int length);
void network_command_append_str(network_command *p, const char *str);

void network_command_append_char(network_command *p, char val);

void network_command_append_byte(network_command *p, my_byte val);

void network_command_append_short(network_command *p, short val);

void network_command_append_int(network_command *p, int val);

//void network_command_append_long(network_command *p, long val);

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

