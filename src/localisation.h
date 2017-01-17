#ifndef LOCALISATION_H
#define LOCALISATION_H

struct locale {
    char *netcli_dcreason_unresponsive;
    char *socket_reject_no_room;
    char *socket_reject_invalid_number;
    
    char *server_shutting_down;
    
    char *server_protection_illegal_cmd_type;
    char *server_protection_unimplemented_cmd_type;
    
    char *client_disconnected;
};

struct locale g_loc;

void init_locale();

#endif /* LOCALISATION_H */

