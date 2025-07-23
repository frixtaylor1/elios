#include <components/components.h>
#include <synch/synch.h>
#include <allocators/alloc.h>
#include <utility/file.h>
#include <stdlib.h>
#include <string.h>

#define BIN_STATE_FILES_PATH "./app_cache/.state%s.bin"

c_string ComponentsName[CMP_COUNT] = {
    [CMP_HEALTH]    = "Health",
    [CMP_COLLISION] = "Collision",
    [CMP_RENDER]    = "Render",
    [CMP_TRANSFORM] = "Transform",
    [CMP_PHYSICS]   = "Physics",
    [CMP_TILE]      = "Tiles"
};

Elios_Private void *HealthComponents[MAX_ENTITIES]    = {0};
Elios_Private void *CollisionComponents[MAX_ENTITIES] = {0};
Elios_Private void *RenderComponents[MAX_ENTITIES]    = {0};
Elios_Private void *TransformComponents[MAX_ENTITIES] = {0};
Elios_Private void *PhysicsComponents[MAX_ENTITIES]   = {0};
Elios_Private void *TileComponents[MAX_ENTITIES]      = {0};

Elios_Private void *ComponentsContainers[CMP_COUNT] = {
    [CMP_HEALTH]    = HealthComponents,
    [CMP_COLLISION] = CollisionComponents,
    [CMP_RENDER]    = RenderComponents,
    [CMP_TRANSFORM] = TransformComponents,
    [CMP_PHYSICS]   = PhysicsComponents,
    [CMP_TILE]      = TileComponents
};

Elios_Private size_t ComponentSizes[CMP_COUNT] = {
    [CMP_HEALTH]    = sizeof (HealthComponent),
    [CMP_COLLISION] = sizeof (CollisionComponent),
    [CMP_RENDER]    = sizeof (RenderComponent),
    [CMP_TRANSFORM] = sizeof (TransformComponent),
    [CMP_PHYSICS]   = sizeof (PhysicsComponent)
};

Elios_Private Elios_Constructor void init_module() {
    mutex_init(CMP_HEALTH);
    mutex_init(CMP_COLLISION);
    mutex_init(CMP_RENDER);
    mutex_init(CMP_TRANSFORM);
    mutex_init(CMP_PHYSICS);
    mutex_init(CMP_TILE);
}

Elios_Private Elios_Destructor void destroy_module() {
    mutex_destroy(CMP_HEALTH);
    mutex_destroy(CMP_COLLISION);
    mutex_destroy(CMP_RENDER);
    mutex_destroy(CMP_TRANSFORM);
    mutex_destroy(CMP_PHYSICS);
    mutex_destroy(CMP_TILE);
}

Elios_Private void *get_component_iml(void **componentContainer, int32 entityId, int32 mutexId) {
    mutex_lock(mutexId);
    void *ptr = componentContainer[entityId];
    mutex_unlock(mutexId);
    return ptr;
}

Elios_Private void assign_component_threadsafe(
    void **componentArray,
    int32 entityId,
    void *content,
    size_t size,
    int32 mutexId
) {
    void *component = mem_alloc(size);
    memcpy(component, content, size);

    mutex_lock(mutexId);
    componentArray[entityId] = component;
    mutex_unlock(mutexId);
}

Elios_Public void *get_health_component(int32 entityId) {
    return get_component_iml(HealthComponents, entityId, CMP_HEALTH);
}

Elios_Public void *get_collision_component(int32 entityId) {
    return get_component_iml(CollisionComponents, entityId, CMP_COLLISION);
}

Elios_Public void *get_transform_component(int32 entityId) {
    return get_component_iml(TransformComponents, entityId, CMP_TRANSFORM);
}

Elios_Public void *get_render_component(int32 entityId) {
    return get_component_iml(RenderComponents, entityId, CMP_RENDER);
}

Elios_Public void *get_physics_component(int32 entityId) {
    return get_component_iml(PhysicsComponents, entityId, CMP_PHYSICS);
}

Elios_Public void *get_tile_component(int32 entityId) {
    return get_component_iml(TileComponents, entityId, CMP_TILE);
}

Elios_Public void add_health_component(int32 entityId, void* content, int32 size) {
    assign_component_threadsafe(HealthComponents, entityId, content, size, CMP_HEALTH);
}

Elios_Public void add_collision_component(int32 entityId, void* content, int32 size) {
    assign_component_threadsafe(CollisionComponents, entityId, content, size, CMP_COLLISION);
}

Elios_Public void add_transform_component(int32 entityId, void *content, int32 size) {
    assign_component_threadsafe(TransformComponents, entityId, content, size, CMP_TRANSFORM);
}

Elios_Public void add_render_component(int32 entityId, void* content, int32 size) {
    assign_component_threadsafe(RenderComponents, entityId, content, size, CMP_RENDER);
}

Elios_Public void add_physics_component(int32 entityId, void* content, int32 size) {
    assign_component_threadsafe(PhysicsComponents, entityId, content, size, CMP_PHYSICS);
}

Elios_Public void add_tile_component(int32 entityId, void *content, int32 size) {
    assign_component_threadsafe(TileComponents, entityId, content, size, CMP_TILE);
}

Elios_Private void format_file_name(const char *base, char *out, size_t maxLen) {
    snprintf(out, maxLen, BIN_STATE_FILES_PATH, base);
}


Elios_Public void save_components_state() {
    char fileName[64];

    ForRange(int32, containerId, 0, CMP_COUNT) {
        format_file_name(ComponentsName[containerId], fileName, sizeof(fileName));

        Elios_Safe_File file = fopen(fileName, "wb");

        IfFalse ((bool) file) {
            ThrowErr(1, "Cannot open file for writing");
        }

        void **container = (void **)ComponentsContainers[containerId];
        size_t componentSize = ComponentSizes[containerId];

        ForRange (int32, entityId, 0, MAX_ENTITIES)
            IfTrue ((bool) container[entityId]) {
                fwrite(&entityId, sizeof(int32), 1, file);
                fwrite(container[entityId], componentSize, 1, file);
            }
        EForRange;
    } EForRange;
}

Elios_Public void reload_components_state() {
    char fileName[64];

    ForRange(int32, containerId, 0, CMP_COUNT) {
        format_file_name(ComponentsName[containerId], fileName, sizeof(fileName));

        Elios_Safe_File file = fopen(fileName, "rb");

        IfFalse ((bool) file) {
            continue;
        }

        void **container = (void **)ComponentsContainers[containerId];
        size_t componentSize = ComponentSizes[containerId];

        int32 entityId;
        WhileTrue ((bool) fread(&entityId, sizeof(int32), 1, file)) {
            void *component = mem_alloc(componentSize);
            IfTrue (fread(component, componentSize, 1, file) != 1) {
                free(component);
                break;
            }
            container[entityId] = component;
        }
    } EForRange;
}
