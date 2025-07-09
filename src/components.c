#include <components/components.h>

cstring ComponentsName[CMP_COUNT + 1] = {
  [1] = "Health",
  [2] = "Collision",
  [3] = "Render"
};

#define $COMPONENT_DEF(enum_name, struct_type, COMPONENTS_ARR_NAMES) void *COMPONENTS_ARR_NAMES[MAX_ENTITIES] = {0};
  $COMPONENT_LIST
#undef X