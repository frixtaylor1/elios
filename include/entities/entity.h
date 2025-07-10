#ifndef entity_H
#define entity_H

#include "../elios.h"
#include <components/components.h>


typedef struct {
  mask components;
  int32 id: 30;
  bool reserved : 1;
  bool alloced : 1;
} Entity;

typedef struct {
  Entity entities[MAX_ENTITIES];
  int32 nbEntities;
  int32 idxCounter;
} EntityManager;

/*
 * Entity functions...
 */

Elios_Public void for_each_component_of_entity(const Entity *entity, void (*callback)(void *));
Elios_Public void add_component(Entity *entity, ComponentType component, void* content);
Elios_Public void *get_component(const Entity *entity, ComponentType type);
Elios_Public void remove_component(Entity *entity, ComponentType component);
Elios_Public bool has_component(const Entity *entity, ComponentType component);
Elios_Public void inspect_entity(const Entity *entity);

/*
 * EntityManager functions...
 */

Elios_Public void clean_up_entity_manager();
Elios_Public int32 add_entity();
Elios_Public Entity *get_entity(const int32 id);
Elios_Public void remove_entity(int32 id);

#endif // entity_H
