#include <systems/systems.h>
#include <components/components.h>
#include <entities/entity.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>
#include <stdio.h>

Elios_Private Camera3D camera = {
    .position = { 50.0f, 50.0f, 150.0f },
    .target = { 0.0f, 0.0f, 0.0f },
    .up = { 0.0f, 1.0f, 0.0f },
    .fovy = 45.0f,
    .projection = CAMERA_PERSPECTIVE
};

Elios_Private int cameraMode = CAMERA_FIRST_PERSON;

Elios_Public Mesh     cubeMesh = {0};
Elios_Public Material cubeMaterial = {0};

Elios_Private void update_camera() {
    IfTrue (IsKeyPressed(KEY_ONE)) {
        cameraMode = CAMERA_FREE;
        camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
    }

    IfTrue (IsKeyPressed(KEY_TWO)) {
        cameraMode = CAMERA_FIRST_PERSON;
        camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
    }

    IfTrue (IsKeyPressed(KEY_THREE)) {
        cameraMode = CAMERA_THIRD_PERSON;
        camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
    }

    IfTrue (IsKeyPressed(KEY_FOUR)) {
        cameraMode = CAMERA_ORBITAL;
        camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
    }

    IfTrue (IsKeyPressed(KEY_P) && camera.projection == CAMERA_PERSPECTIVE) {
        cameraMode = CAMERA_THIRD_PERSON;
        camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
        camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.projection = CAMERA_ORTHOGRAPHIC;
        camera.fovy = 20.0f;
    } else
    IfTrue (IsKeyPressed(KEY_P) && camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        cameraMode = CAMERA_THIRD_PERSON;
        camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
        camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.projection = CAMERA_PERSPECTIVE;
        camera.fovy = 60.0f;
    }

    UpdateCamera(&camera, cameraMode);
}

Elios_Public void physics_system(int threadId, int start, int end) {
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

            float dist = Vector3Distance((Vector3){0.f, 0.f, 0.f}, t->position);

            float dot = Vector3DotProduct(t->position, p->velocity);

            IfTrue (dist >= 40.f) {
                p->velocity = Vector3Scale(p->velocity, -1);
            } else
            IfTrue (dist <= 0.1f && dot < 0) {
                p->velocity = Vector3Scale(p->velocity, -1);
            }
        }
    EForRange;

    (void) threadId;
}

Elios_Public void render_system() {
    update_camera();

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);

    ForRange (int32, id, 0, get_nb_entities())
        const Entity *e = get_entity(id);
        
        IfFalse (((bool) e) && (has_component(e, CMP_TRANSFORM) || has_component(e, CMP_RENDER))) continue;

        const RenderComponent *r = (const RenderComponent *)get_component(e, CMP_RENDER);
        const TransformComponent *t = (const TransformComponent *)get_component(e, CMP_TRANSFORM);

        Vector2 pos = GetWorldToScreen(t->position, camera);
        pos = Vector2Multiply(pos, (Vector2){.x = 0.9f, .y = 0.9f});
        bool visible = pos.x  >= 0 &&
                       pos.x <= GetScreenWidth() &&
                       pos.y  >= 0 &&
                       pos.y <= GetScreenHeight() && 
                       Vector3Distance(camera.position, t->position) < 400;

        IfFalse (visible) {
            continue;
        }

        cubeMaterial.maps[MATERIAL_MAP_DIFFUSE].color = r->color;
        DrawMesh(cubeMesh, cubeMaterial, MatrixTranslate(t->position.x, t->position.y, t->position.z));
    EForRange;

    // DrawGrid(20, 10.0f);

    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

Elios_Public void health_system(int threadId, int start, int end) {
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

// Elios_Private void synch_entities() {
//     ForRange (int32, id, 0, get_nb_entities())
//         const Entity *e = get_entity(id);
//         IfFalse ((bool) e && !e->toRemove && (has_component(e, CMP_TRANSFORM) || has_component(e, CMP_RENDER)))
//             continue;
//     EForRange;
// }

Elios_Public void init_entities() {
    rlSetClipPlanes(0.1, 3000.0);
    cubeMesh = GenMeshCube(.4f, .4f, .4f);
    cubeMaterial = LoadMaterialDefault();

    ForRange (int32, _, 0, 7000)
        int32 id = add_entity();
        Entity *e = get_entity(id);
        IfFalse ((bool) e) ThrowErr(1, "Null entity at ID: %d", id);

        Vector3 pos = {
            GetRandomValue(-5, 5),
            GetRandomValue(-5, 5),
            GetRandomValue(-5, 5)
        };

        const Color col = ColorFromHSV(GetRandomValue(0, 360), 0.8f, 0.9f);

        add_component(e, CMP_TRANSFORM, $void &(TransformComponent){
            .position = pos,
            .rotation = { 0, 0, 0 },
            .scale = { 0.4f, .4f, .4f }
        });

        add_component(e, CMP_RENDER, $void &(RenderComponent){
            .color = col
        });

        Vector3 vel = {
            GetRandomValue(-10, 10) / 1.0f,
            GetRandomValue(-10, 10) / 1.0f,
            GetRandomValue(-10, 10) / 1.0f
        };

        add_component(e, CMP_PHYSICS, $void &(PhysicsComponent){
            .velocity = vel
        });
    EForRange;
}

Elios_Public void destroy_entities() {
    UnloadMesh(cubeMesh);
}