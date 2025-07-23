#ifndef controller_H
#define controller_H

#include <elios.h>
#include <engine/engine.h>
#include <camera/camera.h>
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

Elios_Public void add_cube_callback(void *params) {
	add_cube(params);
}

Elios_Public void create_tile_area_callback(void *from, void *to) {
	create_tile_area(from, to);
}

Elios_Public int get_entity_under_mouse_callback() {
	return get_entity_under_mouse();
}

Elios_Public void disable_camera_control_callback() {
	disable_camera_control();
}

Elios_Public void enable_camera_control_callback() {
	enable_camera_control();
} 

#endif // controller_H