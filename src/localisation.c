#include "localisation.h"

void init_locale() {
    g_loc.netcli_dcreason_unresponsive = "Client did not respond in allowed time";
    g_loc.socket_reject_no_room = "Connecting area is currently full. Try again later";
    g_loc.socket_reject_invalid_number = "Connected to wrong socket. Try again later";
    
    g_loc.server_shutting_down = "Server is shutting down";
    
    g_loc.server_protection_illegal_cmd_type = "Illegal command type provided (%d), do not come back. Thanks";
    g_loc.server_protection_unimplemented_cmd_type = "Command type (%d) handling is not implemented.";
    
    
}