#include <entities/entity.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <allocators/alloc.h>
#include <components/components.h>
#include <synch/synch.h>
#include <threads/threads.h>
#include <entities/entity.h>
#include <components/components.h>
#include <utility/file.h>
#include <stdlib.h>

#define ENTITIES_BIN_NAME "./app_cache/.stateEntities.bin"

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
  ForRange(int32, i, 0, MAX_COMPONENTS_PER_ENTITY)
    IfTrue ((bool) (m & (1LL << i))) {
      printf("1");
    }
    else {
      printf("0");
    }
  EForRange;
  printf("\n");
}

Elios_Private bool keep_searching_entity(const Entity *entity) {
  return entity->alloced && entity < lastAddressEntityManager;
}

Elios_Private Entity* find_free_entity() {
  int id = 0;
  Entity *entity = entityManager.entities;
  WhileTrue (keep_searching_entity(++entity)) ++id;
  IfTrue (entity->alloced) ThrowErr(33, "Max cap reached for allocate entities");
  entity->id = id;
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


Elios_Public void add_component(Entity *entity, int32 component, void *content) {
  IfTrue (has_component(entity, component)) return;
  
  entity->components |= (1LL << component);

  switch (component) {
    case CMP_HEALTH:    add_health_component(entity->id, content, sizeof(HealthComponent));       break;
    case CMP_RENDER:    add_render_component(entity->id, content, sizeof(RenderComponent));       break;
    case CMP_COLLISION: add_collision_component(entity->id, content, sizeof(CollisionComponent)); break;
    case CMP_TRANSFORM: add_transform_component(entity->id, content, sizeof(TransformComponent)); break;
    case CMP_PHYSICS:   add_physics_component(entity->id, content, sizeof(PhysicsComponent));     break;
    case CMP_TILE:      add_tile_component(entity->id, content, sizeof(TileComponent));           break;
    case CMP_COUNT:     break;
  }
}

Elios_Public void *get_component(const Entity *entity, int32 component) {
  IfTrue (has_component(entity, component)) {
    switch (component) {
      case CMP_HEALTH:    return get_health_component(entity->id);    break;
      case CMP_RENDER:    return get_render_component(entity->id);    break;
      case CMP_COLLISION: return get_collision_component(entity->id); break;
      case CMP_TRANSFORM: return get_transform_component(entity->id); break;
      case CMP_PHYSICS:   return get_physics_component(entity->id);   break;
      case CMP_TILE:      return get_tile_component(entity->id);      break;
      case CMP_COUNT:     break;
    }
  }
  return NULL;
}

Elios_Public void remove_component(Entity *entity, int32 component) {
  IfTrue (has_component(entity, component)) {
    entity->components &= ~(1LL << component);
    void* ptr = get_component(entity, component);
    mem_dealloc(ptr);
  }
}

Elios_Public bool has_component(const Entity *entity, int32 component) {
  return entity->components & (1LL << component);
}

Elios_Public void inspect_entity(const Entity *entity) {
  printf("........................................................................................................\n");
  printf("Entity id: %d\n", entity->id);
  print_mask_entity(entity->components);
  int16 componentTypeId = 0;
  ForEach (c_string *, componentName, ComponentsName)
    IfTrue (has_component(entity, componentTypeId)) {
      printf("\t-> %s\n", ComponentsName[componentTypeId]);
    }
    componentTypeId++;
  EForEach;
  printf("........................................................................................................\n");
}

Elios_Public void for_each_component_of_entity(const Entity *entity, ActionComponent callback) {
  int16 componentTypeId = 0;
  ForEach (c_string *, componentName, ComponentsName)
    IfTrue (has_component(entity, componentTypeId)) {
      void *ptr = get_component(entity, componentTypeId++);
      callback(ptr);
    }
  EForEach;
}

Elios_Public void clean_up_entity_manager() {
  mutex_destroy(ENTITY_MUTEX_ID);
  init_entity_manager();
}

Elios_Public int32 add_entity() {
  mutex_lock(ENTITY_MUTEX_ID);
  Entity *entity = find_free_entity();
  entity->alloced = true;
  entityManager.nbEntities++;
  mutex_unlock(ENTITY_MUTEX_ID);
  return entity->id;
}

Elios_Public Entity *get_entity(const int32 id) {
  Entity *res;
  mutex_lock(ENTITY_MUTEX_ID);
  res = &entityManager.entities[id];
  mutex_unlock(ENTITY_MUTEX_ID);
  return res;
}

Elios_Public void remove_entity(int32 id) {
  entityManager.nbEntities--;

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

Elios_Public int32 get_nb_entities() {
  return entityManager.nbEntities;
}

Elios_Public void mark_to_remove_entity(const int32 id) {
  mutex_lock(ENTITY_MUTEX_ID);
  entityManager.entities[id].toRemove = true;
  mutex_unlock(ENTITY_MUTEX_ID);
}

Elios_Public void save_entities_state() {
  Elios_Safe_File file = fopen(ENTITIES_BIN_NAME, "wb");
  IfFalse ((bool) file) ThrowErr(55, "Error opening file.");

  int32 writtenEntities = fwrite(entityManager.entities, sizeof(Entity), entityManager.nbEntities, file);
  
  IfFalse (writtenEntities == entityManager.nbEntities) {
    ThrowErr(56, "Error writting file with entities state.");
  }
}

Elios_Public void reload_entities_state() {
    Elios_Safe_File file = fopen(ENTITIES_BIN_NAME, "rb");
    IfFalse ((bool) file) ThrowErr(55, "Error opening entity state file.");

    init_entity_manager();

    int32 read = fread(entityManager.entities, sizeof(Entity), MAX_ENTITIES, file);

    int32 count = 0;
    ForRange (int32, i, 0, read)
      IfTrue (entityManager.entities[i].alloced)
        count++;
    EForRange;
    entityManager.nbEntities = count;
}
