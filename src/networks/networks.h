#ifndef NETWORKS_H
#define	NETWORKS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#include <semaphore.h>

#include "../settings.h"

#include "netcommand_buffer.h"
#include "network_command.h"
#include "net_client.h"


#define NETWORKS_COMMAND_SIZE 1024
#define NETWORKS_SHUTDOWN_KEY_LENGTH 12

#define NETWORKS_STATUS_NEW 1
#define NETWORKS_STATUS_RUNNING 3
#define NETWORKS_STATUS_END 4


typedef struct networks {
	settings *p_settings;
	
	int server_sock, client_sock, n;
	unsigned int server_addr_len, client_addr_len;
	struct sockaddr_in *local_addr;
	struct sockaddr_in *remote_addr;
	
	int status;
	
	int command_counter;
	
	sem_t *sem_commands_recv;
	netcommand_buffer *netcmd_buf_recv;
	
	sem_t *sem_commands_send;
	netcommand_buffer *netcmd_buf_send;
	network_command *tmp_command;
	
	char shutdown_key[NETWORKS_SHUTDOWN_KEY_LENGTH];
	
	net_client *clients;
	int client_count;
	
	
} networks;

networks *networks_create();

void networks_delete(networks *p_networks);


void networks_handle_message(networks *p_networks, network_command *ncmd);

void networks_send_message(networks *p_networks, network_command *ncmd);

void networks_shutdown(networks *p);


void *networks_receiver_run(void *p_networks);

void *networks_sender_run(void *p_networks);

#endif	/* NETWORKS_H */

