#include <engine/engine.h>
#include <components/components.h>
#include <systems/systems.h>
#include <entities/entity.h>
#include <camera/camera.h>
#include <threads/threads.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <stdio.h>

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
		.mode  = DEV_MODE,
		.props = (EngineProps) {
			.name        = "EEEEEEEEEEEEEEEEEEEE",
			.fps         = 0,
			.maxfps      = 144,
			.timeElapsed = 0
		},
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

Elios_Public void add_cube(void *params) {
	Vector3 pos       = *(Vector3 *) params;
	int32 newEntityId = add_entity();
	Entity *e         = get_entity(newEntityId);

	add_component(e, CMP_RENDER, $void &(RenderComponent) {
		.color = GREEN
	});

	add_component(e, CMP_TRANSFORM, $void &(TransformComponent) {
		.position = pos,
		.rotation = (Vector3) {
			.x = 0.f,
			.y = 0.f,
			.z = 0.f			
		},
		.scale = (Vector3) {
			.x = 5.f,
			.y = 5.f,
			.z = 2.f
		}
	});
}

Vector3 get_mouse_world_position_on_ground() {
    Ray ray = GetMouseRay(GetMousePosition(), *get_camera());

    float t = -ray.position.y / ray.direction.y;

    Vector3 hitPoint = Vector3Add(ray.position, Vector3Scale(ray.direction, t));

    return hitPoint;
}

Elios_Public int get_entity_under_mouse() {
  	Ray ray = GetMouseRay(GetMousePosition(), *get_camera());

  	ForRange (int32, id, 0, get_nb_entities())
		Entity *e = get_entity(id);
		IfFalse ((bool) e && has_component(e, CMP_TRANSFORM)) continue;

		TransformComponent *t = get_component(e, CMP_TRANSFORM);

		float sx = 0.2f, sy = 0.2f, sz = 0.2f;
		IfTrue (has_component(e, CMP_RENDER)) {
		  	sx = sy = sz = 0.2f;
		}

		BoundingBox box = {
		  	.min = Vector3Subtract(t->position, (Vector3){sx, sy, sz}),
		  	.max = Vector3Add(t->position, (Vector3){sx, sy, sz})
		};

		IfTrue (GetRayCollisionBox(ray, box).hit) {
			return id;
		}
  	EForRange;
  return -1;
}

Elios_Public void create_tile_area(void *from, void *to) {
    Vector3 _from = *(Vector3 *) from;
    Vector3 _to   = *(Vector3 *) to;
    
    Vector3 center = {
        .x = (_from.x + _to.x) / 2.f,
        .y = 0.f,
        .z = (_from.z + _to.z) / 2.f
    };

    float sizeX = fabsf(_to.x - _from.x);
    float sizeZ = fabsf(_to.z - _from.z);

    float tileSize = 5.f;
    int32 nbCells = (int32)((sizeX * sizeZ) / (tileSize * tileSize));

    int32 id  = add_entity();
    Entity *e = get_entity(id);

    add_component(e, CMP_TRANSFORM, $void &(TransformComponent){
        .position = center,
        .rotation = {0},
        .scale    = { sizeX, 1.f, sizeZ }
    });

    add_component(e, CMP_RENDER, $void &(RenderComponent){
        .color = LIGHTGRAY
    });

    add_component(e, CMP_TILE, $void &(TileComponent){
        .position = center,
        .size     = tileSize,
        .nbCells  = nbCells
    });
}