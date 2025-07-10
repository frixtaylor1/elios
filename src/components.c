// components.c
#include <components/components.h>
#include <synch/synch.h>
#include <allocators/alloc.h>

#include <string.h>

cstring ComponentsName[CMP_COUNT] = {
    [CMP_HEALTH]    = "Health",
    [CMP_COLLISION] = "Collision",
    [CMP_RENDER]    = "Render"
};

void *HealthComponents[MAX_ENTITIES]    = {0};
void *CollisionComponents[MAX_ENTITIES] = {0};
void *RenderComponents[MAX_ENTITIES]    = {0};

Elios_Private Elios_Constructor void init_module() {
    mutex_init(CMP_HEALTH);
    mutex_init(CMP_COLLISION);
    mutex_init(CMP_RENDER);
}

Elios_Private Elios_Destructor void destroy_module() {
    mutex_destroy(CMP_HEALTH);
    mutex_destroy(CMP_COLLISION);
    mutex_destroy(CMP_RENDER);
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

Elios_Public void *get_render_component(int32 entityId) {
    return get_component_iml(RenderComponents, entityId, CMP_RENDER);
}

Elios_Public void add_health_component(int32 entityId, void* content, int32 size) {
    assign_component_threadsafe(HealthComponents, entityId, content, size, CMP_HEALTH);
}

Elios_Public void add_collision_component(int32 entityId, void* content, int32 size) {
    assign_component_threadsafe(CollisionComponents, entityId, content, size, CMP_COLLISION);
}

Elios_Public void add_render_component(int32 entityId, void* content, int32 size) {
    assign_component_threadsafe(RenderComponents, entityId, content, size, CMP_RENDER);
}