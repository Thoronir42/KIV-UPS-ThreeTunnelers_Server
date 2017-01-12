#ifndef LOCALISATION_H
#define LOCALISATION_H

struct locale {
    char *netcli_dcreason_unresponsive;
    
    char *socket_reject_no_room;
    char *socket_reject_invalid_number;
    
    char *server_shutting_down;
};

struct locale loc;

void init_locale();

#endif /* LOCALISATION_H */

