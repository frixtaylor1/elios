#include <engine/engine.h>

typedef struct Engine {
	EngineModes mode;
} Engine;

Elios_Private Engine engine;


Elios_Public EngineModes get_engine_mode() {
	return engine.mode;
}

Elios_Public void change_engine_mode(EngineModes mode) {
	engine.mode = mode;
}

Elios_Private Elios_Constructor void init_module() {
	engine = (Engine) {.mode = DEV_MODE};
}