#ifndef NETWORK_COMMANDS_H
#define NETWORK_COMMANDS_H

#include "../my_strings.h"

#define NETWORK_COMMAND_HEADER_SIZE 4
#define NETWORK_COMMAND_DATA_LENGTH 512

// NCT as Network Command Type
#define NCT_UNDEFINED 0

#define NCT_LEAD_APPROVE 1
#define NCT_LEAD_DENY 2
#define NCT_LEAD_ECHO_REQUEST 3
#define NCT_LEAD_ECHO_REPLY 4
#define NCT_LEAD_BAD_FORMAT 5
#define NCT_LEAD_INTRODUCE 6
#define NCT_LEAD_REINTRODUCE 7

#define NCT_CONNECTION_FETCH_GAME_LIST 10
#define NCT_CONNECTION_CREATE_GAME 11
#define NCT_CONNECTION_JOIN_GAME 12
#define NCT_CONNECTION_DISCONNECT 20
#define NCT_CONNECTION_GAME_ROOM_LIST 25

#define NCT_MSG_PLAIN 40
#define NCT_MSG_RCON 45

#define NCT_ROOM_SYNC_PHASE 100
#define NCT_ROOM_CONTROL_SIGNAL 101

#define NCT_ROOM_CLIENT_REQUEST_INFO 110
#define NCT_ROOM_CLIENT_INTRODUCE 111
#define NCT_ROOM_CLIENT_LEFT 112

#define NCT_ROOM_KICK_CLIENT 120
#define NCT_ROOM_SET_LEADER 121

#define NCT_ROOM_PLAYER_ATTACH 130
#define NCT_ROOM_PLAYER_DETACH 131
#define NCT_ROOM_PLAYER_MOVE 132
#define NCT_ROOM_PLAYER_SET_COLOR 133

#define NCT_MAP_SPECIFICATION 140
#define NCT_MAP_CHUNK_DATA 141
#define NCT_MAP_CHUNK_REQUEST 142
#define NCT_MAP_BLOCK_CHANGES 143

#define NCT_GAME_CONTROLS_SET 201
#define NCT_GAME_TANK_REQUEST 210
#define NCT_GAME_TANK_INFO 211
#define NCT_GAME_PROJ_ADD 220
#define NCT_GAME_PROJ_REM 221

typedef struct network_command {
    my_byte id;
    short type;
    short _length;
    char data[NETWORK_COMMAND_DATA_LENGTH + 1];

    int client_aid;

} network_command;

void network_command_prepare(network_command *p, short type);

void network_command_strprep(network_command *p, short type, char* message);

int network_command_set_data(network_command *p, const char *str, int length);

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

