#include <entities/entity.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <allocators/alloc.h>
#include <components/components.h>

/*
 * Definitions of private members...
 */

Private EntityManager entityManager = {0};
Private Entity *lastAddressEntityManager;

Private void print_mask_entity(mask m) {
  printf("component mask: ");
  /** an unsingned long long mask can hold 64 components
   * For a mask with the first 3 components looks like this:
   * 000000000000000000000000000000000000000000000000000000000000111
   */
  for (int32 i = 0; i < MAX_COMPONENTS_PER_ENTITY; i++) {
    if (m & (1LL << i)) {
      printf("1");
    } else {
      printf("0");
    }
  }
  printf("\n");
}

Private bool keep_searching_entity(const Entity *entity) {
  return entity->alloced && entity < lastAddressEntityManager;
}

Private bool entity_has_equal_id(const Entity *entity, int32 id) {
  return entity->id == id;
}

Private Entity* find_free_entity() {
  Entity *entity = entityManager.entities;
  WhileTrue (keep_searching_entity(++entity));
  IfTrue (entity->alloced) assert(false && "Max cap reached for allocate entities");

  return entity;
}

Private void init_entity_manager() {
  memset($void entityManager.entities, NULL_ENTITY, MAX_ENTITIES * sizeof(Entity));
  entityManager.nbEntities = 0;
  entityManager.idxCounter = 0;
  lastAddressEntityManager = &entityManager.entities[Arr_Size(entityManager.entities)];
}

/*
 * Definitions of public members...
 */


void add_component(ComponentType component, Entity *entity, void *content) {
    IfTrue (has_component(entity, component)) return;
    entity->components |= (1LL << component);

#define $COMPONENT_DEF(enum_name, struct_type, component_list_name) \
    IfTrue (component == enum_name) { \
        component_list_name[entity->id] = mem_alloc(sizeof(struct_type)); \
        *((struct_type *)component_list_name[entity->id]) = *((struct_type *)content); \
        return; \
    }
    $COMPONENT_LIST
#undef $COMPONENT_DEF
}

void *get_component(ComponentType component, Entity *entity) {
#define $COMPONENT_DEF(enum_name, struct_type, component_list_name) \
    IfTrue (component == enum_name) return component_list_name[entity->id];

    $COMPONENT_LIST
#undef $COMPONENT_DEF

    return NULL;
}

Public void remove_component(Entity *entity, ComponentType component) {
  entity->components &= ~(1LL << component);
}

Public bool has_component(const Entity *entity, ComponentType component) {
  return entity->components & (1LL << component);
}

Public void inspect_entity(const Entity *entity) {
  printf("........................................................................................................\n");
  printf("Entity id: %d\n", entity->id);
  print_mask_entity(entity->components);
  int16 componentTypeId = 0;
  ForEach (cstring *, componentName, ComponentsName) {
    IfTrue (has_component(entity, componentTypeId++)) {
      printf("\t-> %s\n", ComponentsName[componentTypeId]);
    }
  } EForEach;
  printf("........................................................................................................\n");
}

Public void clean_up_entity_manager() {
  init_entity_manager();
}

Public int32 add_entity() {
  Entity *entity = find_free_entity();
  entity->alloced = true;
  entity->id      = entityManager.idxCounter++;
  entityManager.nbEntities++;
  return entity->id;
}

Public Entity *get_entity(int32 id) {
  ForEach(Entity *, entity, entityManager.entities) {
    IfTrue (entity_has_equal_id(entity, id)) {
      return entity;
    }
  } EForEach;
  return NULL_ENTITY;
}

Public void remove_entity(int32 id) {
  Entity* entity     = get_entity(id);
  entity->alloced    = false;
  entity->components = 0;
  entity->reserved   = false;
}

Private Constructor void init_module() {
  init_entity_manager();
} 