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

Elios_Private void render_grid(Vector3 pos, int slices, float spacing)
{
    int halfSlices = slices / 2;
    rlBegin(RL_LINES);
        ForRange (int, i, -halfSlices, halfSlices + 1)
            /** Different colors for the central axies */
            IfTrue (i == 0) rlColor3f(0.5f, 0.5f, 0.5f);
            else rlColor3f(0.75f, 0.75f, 0.75f);

            /** Lines paralles to z axis */
            rlVertex3f(pos.x + i * spacing, pos.y, pos.z - halfSlices * spacing);
            rlVertex3f(pos.x + i * spacing, pos.y, pos.z + halfSlices * spacing);

            /** Lines paralles to z axis */
            rlVertex3f(pos.x - halfSlices * spacing, pos.y, pos.z + i * spacing);
            rlVertex3f(pos.x + halfSlices * spacing, pos.y, pos.z + i * spacing);
        EForRange;
    rlEnd();
}

Elios_Public void render_system() {
    update_camera();

    BeginDrawing();
    ClearBackground(GetColor(0x333388F3));
    BeginMode3D(*get_camera());

    ForRange (int32, id, 0, get_nb_entities())
        const Entity *e = get_entity(id);
        
        IfFalse (((bool) e) && (has_component(e, CMP_TRANSFORM) || has_component(e, CMP_RENDER))) continue;

        const RenderComponent *r = (const RenderComponent *)get_component(e, CMP_RENDER);
        const TransformComponent *t = (const TransformComponent *)get_component(e, CMP_TRANSFORM);

        Vector2 pos = GetWorldToScreen(t->position, *get_camera());
        pos = Vector2Multiply(pos, (Vector2){.x = 0.9f, .y = 0.9f});
        bool visible = pos.x  >= 0 &&
                       pos.x <= GetScreenWidth() &&
                       pos.y  >= 0 &&
                       pos.y <= GetScreenHeight() && 
                       Vector3Distance(get_camera()->position, t->position) < 600;

        IfFalse (visible) continue;

        cubeMaterial.maps[MATERIAL_MAP_DIFFUSE].color = r->color;
        DrawMesh(cubeMesh, cubeMaterial, MatrixTranslate(t->position.x, t->position.y, t->position.z));
    EForRange;

    render_grid((Vector3){.x = -30.f, .y = -10.f, .z = 0.f}, 5, 5.0f);
    render_grid((Vector3){.x = -5.f, .y = -5.f, .z = 0.f}, 5, 5.0f);

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