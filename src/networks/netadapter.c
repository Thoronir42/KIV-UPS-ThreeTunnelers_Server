#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include "net_client.h"
#include "netadapter.h"

#include "../my_strings.h"
#include "network_command.h"

////  THREAD_SELECT - INIT
int netadapter_init_socket(netadapter *p) {
	memset(&p->addr, 0, sizeof (struct sockaddr_in));

	p->socket = socket(AF_INET, SOCK_STREAM, 0);

	p->addr.sin_family = AF_INET;
	p->addr.sin_port = htons(p->port);
	p->addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(p->socket, (struct sockaddr *) &p->addr, \
		sizeof (struct sockaddr_in))) {
		return NETADAPTER_STATUS_BIND_ERROR;
	}
	if (listen(p->socket, NETADAPTER_BACKLOG_SIZE)) {
		return NETADAPTER_STATUS_LISTEN_ERROR;
	}

	return NETADAPTER_STATUS_OK;
}

void netadapter_thread_select_bind_listen(netadapter *adapter) {
	printf("Netadapter: Bind and listen ...");
	adapter->status = netadapter_init_socket(adapter);
	if (adapter->status != NETADAPTER_STATUS_OK) {
		printf("ER\n");
		switch (adapter->status) {
			default:
				printf("Unidentified error on initialising adapter\n");
				break;
			case NETADAPTER_STATUS_BIND_ERROR:
				printf("Failed to bind to port %d\n", adapter->port);
				break;
			case NETADAPTER_STATUS_LISTEN_ERROR:
				printf("Failed to listen at port %d\n", adapter->port);
				break;
		}
	} else {
		printf("OK\n");
	}
}

////  THREAD_SELECT - PROCESS INCOMMING STUFF

int netadapter_process_message(net_client cli, char *buffer, int read_size, network_command *command) {
	int length;

	length = NETADAPTER_BUFFER_SIZE - 2;
	if (read_size < length) {
		length = read_size;
	}

	read(cli.socket, buffer, length);

	printf("Prijato %s\n", buffer);

	return network_command_from_string(command, buffer);

}

void netadapter_thread_select_process_socket(netadapter *adapter, int fd) {
	net_client client;
	network_command command;

	char reverse[NETADAPTER_BUFFER_SIZE];

	memset(adapter->_buffer, 0, NETADAPTER_BUFFER_SIZE);
	// je dany socket v sade fd ze kterych lze cist ?
	if (FD_ISSET(fd, &adapter->tests)) {
		// je to server socket ? prijmeme nove spojeni
		if (fd == adapter->socket) {
			client.socket = accept(adapter->socket, (struct sockaddr *) &client.addr, &client.addr_len);
			FD_SET(client.socket, &adapter->client_socks);
			printf("Pripojen novy klient a pridan do sady socketu\n");
			continue;
		}

		// je to klientsky socket ? prijmem data
		// pocet bajtu co je pripraveno ke cteni
		ioctl(fd, FIONREAD, &client.a2read);
		if (client.a2read > 0) { // mame co cist
			client.socket = fd;
			netadapter_process_message(client, adapter->_buffer, client.a2read, &command);
			memset(reverse, 0, NETADAPTER_BUFFER_SIZE);
			strrev(reverse, command.data, command.length);
			
			memcpy(command.data, reverse, command.length);
			
			netadapter_send_command(&client, command);
			printf("Odeslano %s\n", reverse);
		} else { // na socketu se stalo neco spatneho
			close(fd);
			FD_CLR(fd, &adapter->client_socks);
			printf("Klient se odpojil a byl odebran ze sady socketu\n");
		}
	}
}

////  THREAD_SELECT

void *netadapter_thread_select(void *args) {
	netadapter *adapter = (netadapter *) args;
	int return_value;
	int fd;

	netadapter_thread_select_bind_listen(adapter);
	if (adapter->status != NETADAPTER_STATUS_OK) {
		return NULL;
	}

	// vyprazdnime sadu deskriptoru a vlozime server socket
	FD_ZERO(&adapter->client_socks);
	FD_SET(adapter->socket, &adapter->client_socks);

	while (adapter->status == NETADAPTER_STATUS_OK) {
		adapter->tests = adapter->client_socks;

		// sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
		return_value = select(FD_SETSIZE, &adapter->tests, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
		if (return_value < 0) {
			printf("Select - ERR\n");
			adapter->status = NETADAPTER_STATUS_SELECT_ERROR;
			return NULL;
		}
		// vynechavame stdin, stdout, stderr
		for (fd = NETADAPTER_FD_STD_SKIP; fd < FD_SETSIZE; fd++) {
			netadapter_thread_select_process_socket(adapter, fd);
		}
	}
	adapter->status = NETADAPTER_STATUS_FINISHED;
}

int netadapter_init(netadapter *p, int port) {
	memset(p, 0, sizeof (netadapter));

	p->port = port;

	return 0;
}

int netadapter_send_command(net_client *client, network_command *cmd) {
	char buffer[sizeof (network_command) + 2];
	int a2write;
	a2write = network_command_to_string(buffer, cmd);
	
	buffer[a2write] = '\n';
	buffer[a2write + 1] = '\0';
	write(client->socket, buffer, a2write + 2);
	
	return 0;
}