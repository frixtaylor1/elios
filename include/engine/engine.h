#ifndef engine_H
#define engine_H

#include <elios.h>

typedef enum {
	DEV_MODE = 1,
	GAME_MODE = 2,
} EngineModes;

Elios_Public EngineModes get_engine_mode();
Elios_Public void        change_engine_mode(EngineModes mode);
#endif // engine_H