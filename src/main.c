#include <raylib/raylib.h>
#include <systems/systems.h>
#include <entities/entity.h>
#include <threads/threads.h>
#include <engine/engine.h>

int main() {
    InitWindow(1280, 720, "ELIOS 3D Stress Test");
    init_render_system();
    init_entities();
    thread_pool_init();

    thread_sleep(1500);
    EnableCursor();

    WhileFalse (WindowShouldClose()) {
        IfTrue (get_engine_mode() == GAME_MODE) {
            dispatch_system(&physics_system);
            sync_threads();
        }
        render_system();
    }

    thread_pool_shutdown();
    CloseWindow();
    destroy_entities();
    return 0;
}