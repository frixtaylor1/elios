// components.c
#include <components/components.h>

#define $COMPONENT_DEF(enum_name, struct_type, component_list_name) \
    void *component_list_name[MAX_ENTITIES] = {0};
$COMPONENT_LIST
#undef $COMPONENT_DEF

cstring ComponentsName[CMP_COUNT] = {
    [CMP_HEALTH]    = "Health",
    [CMP_COLLISION] = "Collision",
    [CMP_RENDER]    = "Render"
};
