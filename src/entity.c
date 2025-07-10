#include <entities/entity.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <allocators/alloc.h>
#include <components/components.h>
#include <synch/synch.h>

/*
 * Definitions of private members...
 */

Elios_Private EntityManager entityManager = {0};
Elios_Private Entity *lastAddressEntityManager;

Elios_Private void print_mask_entity(mask m) {
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

Elios_Private bool keep_searching_entity(const Entity *entity) {
  return entity->alloced && entity < lastAddressEntityManager;
}

Elios_Private bool entity_has_equal_id(const Entity *entity, int32 id) {
  return entity->id == id;
}

Elios_Private Entity* find_free_entity() {
  Entity *entity = entityManager.entities;
  WhileTrue (keep_searching_entity(++entity));
  IfTrue (entity->alloced) ThrowErr(33, "Max cap reached for allocate entities");

  return entity;
}

Elios_Private void init_entity_manager() {
  memset($void entityManager.entities, NULL_ENTITY, MAX_ENTITIES * sizeof(Entity));
  entityManager.nbEntities = 0;
  entityManager.idxCounter = 0;
  lastAddressEntityManager = &entityManager.entities[Arr_Size(entityManager.entities)];
}

/*
 * Definitions of public members...
 */


Elios_Public void add_component(Entity *entity, ComponentType component, void *content) {
  IfTrue (has_component(entity, component)) return;
  entity->components |= (1LL << component);

  switch (component) {
    case CMP_HEALTH:    add_health_component(entity->id, content, sizeof(HealthComponent));       break;
    case CMP_RENDER:    add_render_component(entity->id, content, sizeof(RenderComponent));       break;
    case CMP_COLLISION: add_collision_component(entity->id, content, sizeof(CollisionComponent)); break;
    case CMP_COUNT:     break;
  }
}

Elios_Public void *get_component(const Entity *entity, ComponentType component) {
  IfTrue (has_component(entity, component)) {
    switch (component) {
      case CMP_HEALTH:    return get_health_component(entity->id);    break;
      case CMP_RENDER:    return get_render_component(entity->id);    break;
      case CMP_COLLISION: return get_collision_component(entity->id); break;
      case CMP_COUNT:     break;
    }
  }
  return NULL;
}

Elios_Public void remove_component(Entity *entity, ComponentType component) {
  IfTrue (has_component(entity, component)) {
    entity->components &= ~(1LL << component);
    void* ptr = get_component(entity, component);
    mem_dealloc(ptr);
  }
}

Elios_Public bool has_component(const Entity *entity, ComponentType component) {
  return entity->components & (1LL << component);
}

Elios_Public void inspect_entity(const Entity *entity) {
  printf("........................................................................................................\n");
  printf("Entity id: %d\n", entity->id);
  print_mask_entity(entity->components);
  int16 componentTypeId = 0;
  ForEach (cstring *, componentName, ComponentsName) {
    IfTrue (has_component(entity, componentTypeId)) {
      printf("\t-> %s\n", ComponentsName[componentTypeId]);
    }
    componentTypeId++;
  } EForEach;
  printf("........................................................................................................\n");
}

Elios_Public void for_each_component_of_entity(const Entity *entity, void (*callback)(void *)) {
  int16 componentTypeId = 0;
  ForEach (cstring *, componentName, ComponentsName) {
    IfTrue (has_component(entity, componentTypeId)) {
      void *ptr = get_component(entity, componentTypeId++);
      callback(ptr);
    }
  } EForEach;
}

Elios_Public void clean_up_entity_manager() {
  mutex_destroy(ENTITY_MUTEX_ID);
  init_entity_manager();
}

Elios_Public int32 add_entity() {
  mutex_lock(ENTITY_MUTEX_ID);
  Entity *entity = find_free_entity();
  entity->alloced = true;
  entity->id      = entityManager.idxCounter++;
  entityManager.nbEntities++;
  mutex_unlock(ENTITY_MUTEX_ID);
  return entity->id;
}

Elios_Public Entity *get_entity(const int32 id) {
  ForEach(Entity *, entity, entityManager.entities) {
    IfTrue (entity_has_equal_id(entity, id)) {
      return entity;
    }
  } EForEach;
  return NULL_ENTITY;
}

Elios_Public void remove_entity(int32 id) {
  Entity* entity     = get_entity(id);
  entity->alloced    = false;

  for_each_component_of_entity(entity, &mem_dealloc);

  entity->components = 0;
  entity->reserved   = false;
}

Elios_Private Elios_Constructor void init_module() {
  mutex_init(ENTITY_MUTEX_ID);
  init_entity_manager();
} 