#ifndef components_H
#define components_H
#include <elios.h>
#include <synch/synch.h>
#include <raylib/raylib.h>

typedef struct {
    float health;
} HealthComponent;

typedef struct {
    bool    isColliding;
    Vector2 posOfCollision;
} CollisionComponent;

typedef struct {
    Color color;
} RenderComponent;

typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} TransformComponent;

typedef struct {
    float           size;
    int32           nbCells;
    Vector3         position;
} TileComponent;

typedef struct {
    Vector3 velocity;
} PhysicsComponent;

typedef enum {
    CMP_HEALTH = 0,
    CMP_COLLISION,
    CMP_RENDER,
    CMP_TRANSFORM,
    CMP_PHYSICS,
    CMP_TILE,
    CMP_COUNT
} ComponentType;

Elios_Public extern c_string ComponentsName[CMP_COUNT];

Elios_Public void *get_health_component(int32 entityId);
Elios_Public void *get_collision_component(int32 entityId);
Elios_Public void *get_transform_component(int32 entityId);
Elios_Public void *get_render_component(int32 entityId);
Elios_Public void *get_physics_component(int32 entityId);
Elios_Public void *get_tile_component(int32 entityId);

Elios_Public void add_health_component(int32 entityId, void *content, int32 size);
Elios_Public void add_collision_component(int32 entityId, void *content, int32 size);
Elios_Public void add_transform_component(int32 entityId, void *content, int32 size);
Elios_Public void add_render_component(int32 entityId, void *content, int32 size);
Elios_Public void add_physics_component(int32 entityId, void *content, int32 size);
Elios_Public void add_tile_component(int32 entityId, void *content, int32 size);
Elios_Public void save_components_state();
Elios_Public void reload_components_state();

#endif // components_H