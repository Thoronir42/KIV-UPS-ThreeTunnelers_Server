#ifndef NETWORK_COMMANDS_H
#define NETWORK_COMMANDS_H

#include "../my_strings.h"

#define NETWORK_COMMAND_HEADER_SIZE 4
#define NETWORK_COMMAND_DATA_LENGTH 512

#define NET_CMD_UNDEFINED 0

#define NET_CMD_LEAD_APPROVE 1
#define NET_CMD_LEAD_DENY 2
#define NET_CMD_LEAD_STILL_THERE 3
#define NET_CMD_BAD_FORMAT 4

#define NET_CMD_CONNECTION_FETCH_GAME_LIST 10
#define NET_CMD_CONNECTION_CREATE_GAME 11
#define NET_CMD_CONNECTION_JOIN_GAME 12
#define NET_CMD_CONNECTION_RECONNECT 13
#define NET_CMD_CONNECTION_DISCONNECT 20
#define NET_CMD_CONNECTION_GAME_ROOM_LIST 25
#define NET_CMD_CONNECTION_GAME_ROOM_LIST_END 26

#define NET_CMD_MSG_PLAIN 40
#define NET_CMD_MSG_RCON 45

#define NET_CMD_ROOM_WRONG_PHASE 110
#define NET_CMD_ROOM_PLAYER_WHO_IS 111
#define NET_CMD_ROOM_PLAYER_INTRODUCE 112
#define NET_CMD_ROOM_SET_COLOR 113
#define NET_CMD_ROOM_KICK_PLAYER 118
#define NET_CMD_ROOM_PLAYER_LEFT 119
#define NET_CMD_ROOM_SET_LEADER 120
#define NET_CMD_ROOM_START_GAME 125
#define NET_CMD_ROOM_GAME_STARTED 126

#define NET_CMD_GAME_CHUNK_REQUEST 140
#define NET_CMD_GAME_CHUNK_DATA 141
#define NET_CMD_GAME_CHUNK_VERIFY 142

#define NET_CMD_GAME_CONTROLS_SET 201
#define NET_CMD_GAME_TANK_REQUEST 210
#define NET_CMD_GAME_TANK_INFO 211
#define NET_CMD_GAME_PROJ_ADD 220
#define NET_CMD_GAME_PROJ_REM 221

typedef struct network_command {
    my_byte id;
    short type;
    short _length;
    char data[NETWORK_COMMAND_DATA_LENGTH + 1];

    short client_aid;

} network_command;

void network_command_prepare(network_command *p, short id, short type);

int network_command_from_string(network_command *dest, char *src, int length);

int network_command_to_string(char *dest, network_command *src);

void network_command_print(const char *label, const network_command *command);

#endif /* NETWORK_COMMANDS_H */

