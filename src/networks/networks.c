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
#include "../settings.h"

networks* networks_create(settings *p_settings) {
	networks *tmp = malloc(sizeof (networks));
	tmp->p_settings = p_settings;
	
	tmp->local_addr = malloc(sizeof (struct sockaddr_in));
	tmp->remote_addr = malloc(sizeof (struct sockaddr_in));


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
		return NULL;
	}

	tmp->status = NETWORKS_STATUS_NEW;
	return tmp;
}

void networks_delete(networks* p_networks) {
	free(p_networks->local_addr);
	free(p_networks->remote_addr);
	free(p_networks);
}

int networks_keep_running(networks *p_networks) {
	return p_networks->status & 1;
}

void *networks_run(networks *p_networks) {
	int n = 0;
	char *ch = p_networks->buffer_command;
	
	while (networks_keep_running(p_networks)) {
		printf("Server ceka na %d. data\n", ++n);

		p_networks->client_addr_len = sizeof (struct sockaddr_in);
		n = recvfrom(p_networks->server_sock, p_networks->buffer_command, 1, 0, (struct sockaddr*) p_networks->remote_addr, &p_networks->client_addr_len);

		printf("Pripojil se klient\n");
		printf("Klient poslal = %s\n", ch);

		ch++;
		sleep(5);

		printf("Server odesila = %s\n", ch);
		n = sendto(p_networks->server_sock, ch, 1, 0, (struct sockaddr*) p_networks->remote_addr, p_networks->client_addr_len);

		close(p_networks->client_sock);

		printf("Network is still alive\n");
		fgets(p_networks->buffer_command, NETWORKS_BUFFER_SIZE, stdin);
		printf(p_networks->buffer_command);
	}
	
	return NULL;
}