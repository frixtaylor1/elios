#include <raylib/raylib.h>
#include <systems/systems.h>
#include <entities/entity.h>
#include <threads/threads.h>
#include <engine/engine.h>
#include <ui/ui.h>
#include <stdio.h>

int main() {
    InitWindow(1280, 720, "ELIOS 3D Stress Test");

    init_entities();
    init_ui();
    thread_pool_init();

    thread_sleep(1500);
    IfTrue (get_engine_mode() == DEV_MODE) {
        EnableCursor();
    }

    IfTrue (get_engine_mode() == GAME_MODE) {
        DisableCursor();
    }

     WhileFalse (WindowShouldClose()) {
        IfTrue (get_engine_mode() == GAME_MODE) {
            dispatch_system(&physics_system);
            sync_threads();
        }

        update_ui();
        render_ui();

        render_system();
    }

    thread_pool_shutdown();
    CloseWindow();
    destroy_entities();
    return 0;
}