#ifndef controller_H
#define controller_H

#include <elios.h>
#include <engine/engine.h>
#include <stdio.h>

Elios_Public void change_mode_callback(void *) {
	EngineModes mode = get_engine_mode() == DEV_MODE ? GAME_MODE : DEV_MODE;
	change_engine_mode(mode);

	printf("%s\n", "Se está clickeando con efectividad!!!");
}

#endif // controller_H