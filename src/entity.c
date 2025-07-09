#include <entities/entity.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*
 * Definitions of private members...
 */

Private EntityManager entityManager = {0};

Private cstring ComponentsName[4] = {
  "None",
  "Health",
  "Collision",
  "Render"
};

Private void print_mask_entity(mask m) {
  printf("component mask: ");
  for (int32 i = 63; i > 0; i--) {
    if (m & (1LL << i)) {
      printf("1");
    } else {
      printf("0");
    }
  }
  printf("\n");
}

Private bool entity_has_equal_id(Entity *entity, int32 id) {
  return entity->id == id;
}

Private Entity* find_free_entity() {
  Entity *entity      = &entityManager.entities[0];
  Entity *lastAddress = &entityManager.entities[Arr_Size(entityManager.entities)];
  WhileTrue (entity->alloced && entity++ < lastAddress);
  IfTrue (entity->alloced) assert(false && "Max cap reached for allocate entities");

  return entity;  
}

/*
 * Definitions of public members...
 */

Public void add_component(Entity *entitiy, ComponentType component) {
  entitiy->components |= (1LL << component);
}

Public void remove_component(Entity *entitiy, ComponentType component) {
  entitiy->components &= ~(1LL << component);
}

Public bool has_component(Entity *entitiy, ComponentType component) {
  return entitiy->components & (1LL << component);
}

Public void init_entity_manager() {
  memset($void entityManager.entities, Null_Entity, Max_Entities * sizeof(Entity));
  entityManager.nbEntities = 0;
  entityManager.idxCounter = 0;
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
  return Null_Entity;
}

Public void remove_entity(int32 id) {
  Entity* entity     = get_entity(id);
  entity->alloced    = false;
  entity->components = 0;
  entity->reserved   = false;
}

Public void inspect_entity(Entity *entitiy) {
  printf("........................................................................................................\n");
  printf("Entity id: %d\n", entitiy->id);
  print_mask_entity(entitiy->components);
  int16 componentTypeId = 0;
  ForEach (cstring *, componentName, ComponentsName) {
    IfTrue (has_component(entitiy, ++componentTypeId)) {
      printf("\t-> %s\n", ComponentsName[componentTypeId]);
    }
  } EForEach;
  printf("........................................................................................................\n");
}