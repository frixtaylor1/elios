
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
#define $COMPONENT_DEF(enum_name, struct_type, COMPONENTS_ARR_NAMES) enum_name,
    $COMPONENT_LIST
#undef $COMPONENT_DEF
    CMP_COUNT
} ComponentType;

#define $COMPONENT_DEF(enum_name, struct_type, COMPONENTS_ARR_NAMES) \
    void *COMPONENTS_ARR_NAMES[MAX_ENTITIES] = {0};
#undef $COMPONENT_DEF

extern cstring ComponentsName[CMP_COUNT + 1];

#define $COMPONENT_DEF(enum_name, struct_type, COMPONENTS_ARR_NAMES) \
    typedef struct struct_type struct_type;   \
    extern void *COMPONENTS_ARR_NAMES[MAX_ENTITIES];
#undef $COMPONENT_DEF

extern void *HealthComponents[MAX_ENTITIES];
extern void *CollisionComponents[MAX_ENTITIES];
extern void *RenderComponents[MAX_ENTITIES];

#endif // components_H