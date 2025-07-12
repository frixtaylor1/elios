#include <threads/threads.h>
#include <entities/entity.h>
#include <components/components.h>
#include <stdio.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>

Elios_Private Camera3D camera = {
    .position = { 50.0f, 50.0f, 150.0f },
    .target = { 0.0f, 0.0f, 0.0f },
    .up = { 0.0f, 1.0f, 0.0f },
    .fovy = 45.0f,
    .projection = CAMERA_PERSPECTIVE
};

Mesh cubeMesh = {0};
Material cubeMaterial = {0};

void health_system(int threadId, int start, int end) {
    ForRange (int, id, start, end)
        Entity *e = get_entity(id);
        IfFalse (!!e) continue;
        IfTrue (has_component(e, CMP_HEALTH)) {
            HealthComponent *hc = get_component(e, CMP_HEALTH);
            hc->health -= 1.1f;
        }
    EForRange;
    (void) threadId;
}

void render_system() {
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);
    ForRange (int32, id, 0, get_nb_entities())
        Entity *e = get_entity(id);
        RenderComponent    *r = get_component(e, CMP_RENDER);
        TransformComponent *t = get_component(e, CMP_TRANSFORM);
        DrawMesh(cubeMesh, cubeMaterial, MatrixTranslate(t->position.x, t->position.y, t->position.z));
        cubeMaterial.maps[MATERIAL_MAP_DIFFUSE].color = r->color;

    EForRange;

    DrawGrid(20, 10.0f);

    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void physics_system(int threadId, int start, int end) {
    float dt = GetFrameTime();
    ForRange (int, id, start, end)
        Entity *e = get_entity(id);
        IfFalse (!!e) continue;

        IfTrue (has_component(e, CMP_TRANSFORM) && has_component(e, CMP_PHYSICS)) {
            TransformComponent *t = get_component(e, CMP_TRANSFORM);
            PhysicsComponent *p = get_component(e, CMP_PHYSICS);

            t->position.x += p->velocity.x * dt;
            t->position.y += p->velocity.y * dt;
            t->position.z += p->velocity.z * dt;
        }
    EForRange;

    (void) threadId;
}

void init_entities() {
    rlSetClipPlanes(0.1, 3000.0);
    cubeMesh = GenMeshCube(1.5f, 1.5f, 1.5f);
    cubeMaterial = LoadMaterialDefault();
    cubeMaterial.maps[MATERIAL_MAP_METALNESS].color = RED;

    ForRange (int32, i, 0, MAX_ENTITIES - 1)
        int32 id = add_entity();
        Entity *e = get_entity(id);
        IfFalse (!!e) ThrowErr(1, "Null entity at ID: %d", id);

        Vector3 pos = {
            GetRandomValue(-50, 50),
            GetRandomValue(-50, 50),
            GetRandomValue(-50, 50)
        };

        Color col = ColorFromHSV(GetRandomValue(0, 360), 0.8f, 0.9f);

        add_component(e, CMP_TRANSFORM, $void &(TransformComponent){
            .position = pos,
            .rotation = { 0, 0, 0 },
            .scale = { 1.5f, 1.5f, 1.5f }
        });

        add_component(e, CMP_RENDER, $void &(RenderComponent){
            .color = col
        });

        Vector3 vel = {
            GetRandomValue(-10, 10) / 10.0f,
            GetRandomValue(-10, 10) / 10.0f,
            GetRandomValue(-10, 10) / 10.0f
        };

        add_component(e, CMP_PHYSICS, $void &(PhysicsComponent){
            .velocity = vel
        });
    EForRange;
}

int main() {
    InitWindow(1280, 800, "ELIOS 3D Stress Test");

    init_entities();

    thread_pool_init();
    thread_sleep(1500);

    WhileFalse (WindowShouldClose()) {
        dispatch_system(&health_system);
        sync_threads();

        dispatch_system(&physics_system);
        sync_threads();

        render_system();
    }

    UnloadMesh(cubeMesh);

    CloseWindow();
    thread_pool_shutdown();
    return 0;
}
