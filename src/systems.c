#include <systems/systems.h>
#include <components/components.h>
#include <entities/entity.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>
#include <ui/ui.h>
#include <camera/camera.h>
#include <stdio.h>

Elios_Public  Mesh     cubeMesh     = {0};
Elios_Public  Material cubeMaterial = {0};

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

Elios_Private void draw_tile_grid(Vector3 center, Vector3 scale, float cellSize) {
    int32 cellsX = (int)(scale.x / cellSize);
    int32 cellsZ = (int)(scale.z / cellSize);

    float startX = center.x - (scale.x / 2);
    float startZ = center.z - (scale.z / 2);

    rlBegin(RL_LINES);
    rlColor4f(0.7f, 0.7f, 0.7f, 0.4f);

    ForRange (int32, i, 0, cellsX + 1)
        float x = startX + i * cellSize;
        rlVertex3f(x, center.y + 0.01f, startZ);
        rlVertex3f(x, center.y + 0.01f, startZ + scale.z);
    EForRange;

    ForRange (int32, j, 0, cellsZ + 1)
        float z = startZ + j * cellSize;
        rlVertex3f(startX, center.y + 0.01f, z);
        rlVertex3f(startX + scale.x, center.y + 0.01f, z);
    EForRange;

    rlEnd();
}

Elios_Private void render_grid(const Entity *e) {
    IfTrue (has_component(e, CMP_TILE)) {
        const TileComponent      *tile               = (const TileComponent *)get_component(e, CMP_TILE);
        const TransformComponent *transformComponent = (const TransformComponent *)get_component(e, CMP_TRANSFORM);

        Color tileColor = (Color){100, 150, 100, 40};
        DrawCubeV(transformComponent->position, transformComponent->scale, tileColor);
        draw_tile_grid(transformComponent->position, transformComponent->scale, tile->size);
    }
}

Elios_Private bool is_in_viewport(Vector3 position) {
    const Camera3D *cam = get_camera();

    Vector3 toEntity = Vector3Subtract(position, cam->position);
    float dist = Vector3Length(toEntity);

    if (dist > 1000.f || dist < 0.01f) return false;

    Vector3 forward = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
    float angle = Vector3Angle(toEntity, forward);

    float fovY = 45.0f * DEG2RAD;
    float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
    float fovX = 2.0f * atanf(tanf(fovY / 2.0f) * aspect);

    return angle <= (fovX / 2.0f);
}

Elios_Private void render_cube_mesh(const Vector3 *pos, const Color *color) {
    cubeMaterial.maps[MATERIAL_MAP_DIFFUSE].color = *color;
    DrawMesh(cubeMesh, cubeMaterial, MatrixTranslate(pos->x, pos->y, pos->z));
}

Elios_Public void render_system() {
    update_camera();

    BeginDrawing();
    ClearBackground(GetColor(0x333388F3));
    BeginMode3D(*get_camera());

    ForRange (int32, id, 0, get_nb_entities())
        const Entity *e = get_entity(id);
        
        IfFalse (((bool) e) && (has_component(e, CMP_RENDER))) continue;

        const RenderComponent    *r = (const RenderComponent *)get_component(e, CMP_RENDER);
        const TransformComponent *t = (const TransformComponent *)get_component(e, CMP_TRANSFORM);

        IfFalse (is_in_viewport(t->position)) continue;

        render_cube_mesh(&t->position, &r->color);
        render_grid(e);
    EForRange;

    EndMode3D();
    
    update_ui();
    render_ui();

    EndDrawing();
}

Elios_Public void health_system(int threadId, int start, int end) {
    ForRange (int, id, start, end)
        Entity *e = get_entity(id);
        IfFalse ((bool) e) continue;
        IfTrue (has_component(e, CMP_HEALTH)) {
            HealthComponent *hc = get_component(e, CMP_HEALTH);
            hc->health -= 1.1f;
        }
    EForRange;
    (void) threadId;
}

Elios_Public void init_entities() {
    rlSetClipPlanes(0.1, 3000.0);
    cubeMesh = GenMeshCube(.4f, .4f, .4f);
    cubeMaterial = LoadMaterialDefault();

    // ForRange (int32, _, 0, 7000)
    //     int32 id = add_entity();
    //     Entity *e = get_entity(id);
    //     IfFalse ((bool) e) ThrowErr(1, "Null entity at ID: %d", id);

    //     Vector3 pos = {
    //         GetRandomValue(-5, 5),
    //         GetRandomValue(-5, 5),
    //         GetRandomValue(-5, 5)
    //     };

    //     const Color col = ColorFromHSV(GetRandomValue(0, 360), 0.8f, 0.9f);

    //     add_component(e, CMP_TRANSFORM, $void &(TransformComponent){
    //         .position = pos,
    //         .rotation = { 0, 0, 0 },
    //         .scale = { 0.4f, .4f, .4f }
    //     });

    //     add_component(e, CMP_RENDER, $void &(RenderComponent){
    //         .color = col
    //     });

    //     Vector3 vel = {
    //         GetRandomValue(-10, 10) / 1.0f,
    //         GetRandomValue(-10, 10) / 1.0f,
    //         GetRandomValue(-10, 10) / 1.0f
    //     };

    //     add_component(e, CMP_PHYSICS, $void &(PhysicsComponent){
    //         .velocity = vel
    //     });
    // EForRange;
}

Elios_Public void init_render_system() {
    init_ui();
}

Elios_Public void destroy_entities() {
    UnloadMesh(cubeMesh);
}
