#include <engine/engine.h>
#include <components/components.h>
#include <systems/systems.h>
#include <entities/entity.h>
#include <threads/threads.h>
#include <raylib/raylib.h>

typedef struct {
	c_string name;
	int32    fps;
	int32    maxfps;
	int32    timeElapsed;
} EngineProps;

typedef struct Engine {
	EngineProps props;
	EngineModes mode;
} Engine;

Elios_Private Engine engine;

Elios_Public void init_engine_obj() {
	engine = (Engine) {
		.props = (EngineProps) {
			.name = "Elios Engine",
			.fps = 0,
			.maxfps = 144,
			.timeElapsed = 0
		},
		.mode = DEV_MODE
	};
}

Elios_Public EngineModes get_engine_mode() {
	return engine.mode;
}

Elios_Public void change_engine_mode(EngineModes mode) {
	engine.mode = mode;
}

Elios_Public void save_engine_state() {
	change_engine_mode(DEV_MODE);

	save_entities_state();
	save_components_state();
}

Elios_Public void reload_engine_state() {
	change_engine_mode(DEV_MODE);

	reload_entities_state();
	reload_components_state();
}

Elios_Private Elios_Constructor void constructor_module() {
	init_engine_obj();
}

Elios_Private void init_engine_modules() {
    InitWindow(1280, 720, "ELIOS 3D Stress Test");
    init_render_system();
    init_entities();
    thread_pool_init();

    thread_sleep(1500);

    IfTrue (get_engine_mode() == DEV_MODE) {
        EnableCursor();
    }

    IfTrue (get_engine_mode() == GAME_MODE) {
        DisableCursor();
    }
}

Elios_Private void unload_engine_modules() {
    thread_pool_shutdown();
    CloseWindow();
    destroy_entities();
}

Elios_Private void main_loop() {
	WhileFalse (WindowShouldClose()) {
        IfTrue (get_engine_mode() == GAME_MODE) {
            dispatch_system(&physics_system);
            sync_threads();
        }
        render_system();
    }
}

Elios_Public void run_engine() {
	init_engine_modules();
	main_loop();
    unload_engine_modules();
}