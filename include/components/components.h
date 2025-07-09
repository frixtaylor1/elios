
#ifndef components_H
#define components_H
#include <elios.h>

#ifndef Vector2
typedef struct {
    float x, y;
} Vector2;
#endif

typedef struct {
    float health;
} HealthComponent;

typedef struct {
    bool    isColliding;
    Vector2 posOfCollision;
} CollisionComponent;

typedef struct {
    Vector2 pos;
} RenderComponent;

#define $COMPONENT_LIST \
    $COMPONENT_DEF(CMP_HEALTH,    HealthComponent,    HealthComponents) \
    $COMPONENT_DEF(CMP_COLLISION, CollisionComponent, CollisionComponents) \
    $COMPONENT_DEF(CMP_RENDER,    RenderComponent,    RenderComponents)

typedef enum {
#define $COMPONENT_DEF(enum_name, struct_type, component_list_name) enum_name,
    $COMPONENT_LIST
#undef $COMPONENT_DEF
    CMP_COUNT
} ComponentType;

extern cstring ComponentsName[CMP_COUNT];

#define $COMPONENT_DEF(enum_name, struct_type, component_list_name) \
    extern void *component_list_name[MAX_ENTITIES];
    $COMPONENT_LIST
#undef $COMPONENT_DEF

#endif // components_H