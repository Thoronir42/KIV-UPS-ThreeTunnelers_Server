#include <stdlib.h>
#include <stdio.h>

#include "engine.h"

#include "../logger.h"

int _exe_solo_undefined ENGINE_HANDLE_FUNC_HEADER {
    return 1;
}

int _exe_solo_lead_disconnect ENGINE_HANDLE_FUNC_HEADER {
    glog(LOG_INFO, "Processing disconnect command(%d) [%s]", str_scanner_rest_length(sc), str_scanner_rest(sc));
    
    return 0;
}

void _engine_init_solo_commands(int (**command_handle_func)ENGINE_HANDLE_FUNC_HEADER) {
    command_handle_func[NCT_UNDEFINED] = 
    command_handle_func[NCT_LEAD_DISCONNECT] = &_exe_solo_lead_disconnect;
}