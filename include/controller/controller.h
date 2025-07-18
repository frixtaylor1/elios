#ifndef controller_H
#define controller_H

#include <elios.h>
#include <engine/engine.h>
#include <stdio.h>

Elios_Public void change_mode_callback(void *) {
	EngineModes mode = get_engine_mode() == DEV_MODE ? GAME_MODE : DEV_MODE;
	change_engine_mode(mode);
}

Elios_Public void save_engine_state_callback(void *) {
	save_engine_state();
}

Elios_Public void reload_engine_state_callback(void *) {
	reload_engine_state();
}

#endif // controller_H