#ifndef NETWORK_COMMANDS_H
#define	NETWORK_COMMANDS_H

#include "net_client.h"
#include "../my_strings.h"

#define NETWORK_COMMAND_DATA_LENGTH 512

#define NET_CMD_LEAD_APPROVE 1
#define NET_CMD_LEAD_DENY 2
#define NET_CMD_LEAD_STILL_THERE 3

#define NET_CMD_CONNECTION_FETCH_LOBBIES 10
#define NET_CMD_CONNECTION_CREATE_LOBBY 11
#define NET_CMD_CONNECTION_JOIN_LOBBY 12
#define NET_CMD_CONNECTION_RECONNECT 13
#define NET_CMD_CONNECTION_DISCONNECT 49
#define NET_CMD_CONNECTION_INCORRECT_PHASE 80
#define NET_CMD_CONNECTION_GAME_ROOM_LIST 90
#define NET_CMD_CONNECTION_GAME_ROOM_LIST_END 91
#define NET_CMD_CONNECTION_PLAYER_DISCONNECTED 99

typedef struct network_command {
	my_byte id;
	short type;
	short length;
	char data[NETWORK_COMMAND_DATA_LENGTH + 1];
	
	net_client *client;
	
} network_command;

void network_command_prepare(network_command *p, short id, short type);

int network_command_from_string(network_command *dest, char *src);

int network_command_to_string(char *dest, network_command *src);

#endif	/* NETWORK_COMMANDS_H */

