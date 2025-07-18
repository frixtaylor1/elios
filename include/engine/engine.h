#ifndef engine_H
#define engine_H

#include <elios.h>

typedef enum {
	DEV_MODE = 1,
	GAME_MODE = 2,
} EngineModes;

Elios_Public void        init_engine();
Elios_Public EngineModes get_engine_mode();
Elios_Public void        change_engine_mode(EngineModes mode);
Elios_Public void        save_engine_state();
Elios_Public void        reload_engine_state();
Elios_Public void        run_engine();
#endif // engine_H