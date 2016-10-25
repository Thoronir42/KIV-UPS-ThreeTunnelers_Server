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

int netadapter_bind_socket(netadapter *p) {
    memset(&p->addr, 0, sizeof (struct sockaddr_in));

    p->socket = socket(AF_INET, SOCK_STREAM, 0);

    p->addr.sin_family = AF_INET;
    p->addr.sin_port = htons(p->port);
    p->addr.sin_addr.s_addr = INADDR_ANY;

    return bind(p->socket, (struct sockaddr *) &p->addr, \
		sizeof (struct sockaddr_in));
}

void *netadapter_thread_select(void *args) {
    netadapter *adapter = (netadapter *) args;
    int return_value;
    net_client client;
    int fd;
    int a2read;

    if (netadapter_bind_socket(adapter) == 0) {
        printf("Bind - OK\n");
    } else {
        printf("Bind - ER\n");
        adapter->status = NETADAPTER_STATUS_BIND_ERROR;
        return NULL;
    }

    if (listen(adapter->socket, 5) == 0) {
        printf("Listen - OK\n");
    } else {
        printf("Listen - ER\n");
        adapter->status = NETADAPTER_STATUS_LISTEN_ERROR;
        return NULL;
    }

    // vyprazdnime sadu deskriptoru a vlozime server socket
    FD_ZERO(&adapter->client_socks);
    FD_SET(adapter->socket, &adapter->client_socks);

    for (;;) {
        adapter->tests = adapter->client_socks;

        // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
        return_value = select(FD_SETSIZE, &adapter->tests, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
        if (return_value < 0) {
            printf("Select - ERR\n");
            adapter->status = NETADAPTER_STATUS_SELECT_ERROR;
            return NULL;
        }
        // vynechavame stdin, stdout, stderr
        for (fd = 3; fd < FD_SETSIZE; fd++) {
            // je dany socket v sade fd ze kterych lze cist ?
            if (FD_ISSET(fd, &adapter->tests)) {
                // je to server socket ? prijmeme nove spojeni
                if (fd == adapter->socket) {
                    client.socket = accept(adapter->socket, (struct sockaddr *) &client.addr, &client.addr_len);
                    FD_SET(client.socket, &adapter->client_socks);
                    printf("Pripojen novy klient a pridan do sady socketu\n");
                }// je to klientsky socket ? prijmem data
                else {
                    // pocet bajtu co je pripraveno ke cteni
                    ioctl(fd, FIONREAD, &a2read);
                    // mame co cist
                    if (a2read > 0) {
                        recv(fd, &adapter->cbuf, 1, 0);
                        printf("Prijato %c\n", adapter->cbuf);
                        read(fd, &adapter->cbuf, 1);
                        printf("Prijato %c\n", adapter->cbuf);
                    }// na socketu se stalo neco spatneho
                    else {
                        close(fd);
                        FD_CLR(fd, &adapter->client_socks);
                        printf("Klient se odpojil a byl odebran ze sady socketu\n");
                    }
                }
            }
        }
    }
    adapter->status = NETADAPTER_STATUS_FINISHED;
}

int netadapter_init(netadapter *p, int port){
    memset(p, 0, sizeof(netadapter));
    
    p->port = port;
    
    return 0;
}