#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#include "networks.h"
#include "network_command.h"
#include "../settings.h"

int networks_setup(networks *tmp, settings *p_settings){
	tmp->server_sock = socket(AF_INET, SOCK_DGRAM, 0);

	memset(tmp->local_addr, 0, sizeof (struct sockaddr_in));

	tmp->local_addr->sin_family = AF_INET;
	tmp->local_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	tmp->local_addr->sin_port = htons(p_settings->port);

	tmp->server_addr_len = sizeof ( struct sockaddr_in);

	printf("Attempting to bind port %d ...", p_settings->port);
	
	if (bind(tmp->server_sock, (struct sockaddr *) tmp->local_addr, tmp->server_addr_len) != 0) {
		printf("ok\n");
	} else {
		printf("error\n");
		return 1;
	}
	
	return 0;
}

networks* networks_create(settings *p_settings) {
	networks *tmp = malloc(sizeof (networks));
	tmp->local_addr = malloc(sizeof (struct sockaddr_in));
	tmp->remote_addr = malloc(sizeof (struct sockaddr_in));
	tmp->sem_commands_recv = malloc(sizeof(sem_t));
	tmp->sem_commands_send = malloc(sizeof(sem_t));
	tmp->tmp_command = malloc(sizeof(network_command));
	
	
	tmp->p_settings = p_settings;
	tmp->command_counter = 0;
	
	if (!networks_setup(tmp, p_settings)) {
		return NULL;
	}
	
	sem_init(tmp->sem_commands_recv, 0, p_settings->NETWORKS_RECV_BUFFER_SIZE);
	sem_init(tmp->sem_commands_send, 0, p_settings->NETWORKS_SEND_BUFFER_SIZE);

	tmp->status = NETWORKS_STATUS_NEW;
	return tmp;
}

void networks_delete(networks* p_networks) {	
	sem_destroy(p_networks->sem_commands_recv);
	sem_destroy(p_networks->sem_commands_send);
	
	free(p_networks->local_addr);
	free(p_networks->remote_addr);
	free(p_networks->sem_commands_recv);
	free(p_networks->sem_commands_send);
	free(p_networks->tmp_command);
	free(p_networks);
}

void networks_handle_message(networks *p_networks, network_command *ncmd){
	netcommand_buffer_put(p_networks->netcmd_buf_recv, ncmd);
	sem_post(p_networks->sem_commands_recv);
}

void networks_send_message(networks *p_networks, network_command *ncmd){
	netcommand_buffer_put(p_networks->netcmd_buf_send, ncmd);
	sem_post(p_networks->sem_commands_send);
}

void networks_shutdown(networks *p){
	int i;
	static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (i = 0; i < NETWORKS_SHUTDOWN_KEY_LENGTH; ++i) {
        p->shutdown_key[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
	p->status = NETWORKS_STATUS_END;
	
	network_command_prepare(p->tmp_command, ++p->command_counter, NET_CMD_SERVER_SHUTDOWN);
	memcpy(p->tmp_command->content + NETWORK_COMMAND_BODY_OFFSET, p->shutdown_key, NETWORKS_SHUTDOWN_KEY_LENGTH);
	
	networks_send_message(p, p->tmp_command);
}


int networks_keep_running(networks *p_networks) {
	return p_networks->status & 1;
}

void *networks_receiver_run(void *args){
	
	networks *p_networks = (networks *)args;
	
	int n = 0;
	network_command buffer;
	network_command *p_buffer = &buffer;
	
	while (networks_keep_running(p_networks)) {
		printf("Server ceka na %d. data\n", ++n);

		p_networks->client_addr_len = sizeof (struct sockaddr_in);
		n = recvfrom(p_networks->server_sock, buffer.content, 1, 0, (struct sockaddr*) p_networks->remote_addr, &p_networks->client_addr_len);

		printf("Pripojil se klient\n");
		printf("Klient poslal = %s\n", buffer);
		
		networks_handle_message(p_networks, p_buffer);

	}
	
	return NULL;
}

void *networks_sender_run(void *args){
	
	networks *p_networks = (networks *)args;
	
	network_command buffer;
	network_command *p_buffer;
	int n;
	
	while(networks_keep_running(p_networks)){
		sem_wait(p_networks->sem_commands_send);
		
		memcpy(p_buffer, p_networks->netcmd_buf_send, sizeof(network_command));
		
		buffer.content[0]++;

		printf("Server odesila = %s\n", buffer.content);
		n = sendto(p_networks->server_sock, buffer.content, 1, 0, (struct sockaddr*) p_networks->remote_addr, p_networks->client_addr_len);

		close(p_networks->client_sock);

		printf("Network is still alive\n");
		fgets(buffer.content, NETWORKS_COMMAND_SIZE, stdin);
		printf(buffer.content);
		
	}
}
