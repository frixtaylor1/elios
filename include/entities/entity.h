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

Public void for_each_component_of_entity(const Entity *entity, void (*callback)(void *));
Public void add_component(Entity *entity, ComponentType component, void* content);
Public void *get_component(const Entity *entity, ComponentType type);
Public void remove_component(Entity *entity, ComponentType component);
Public bool has_component(const Entity *entity, ComponentType component);
Public void inspect_entity(const Entity *entity);

/*
 * EntityManager functions...
 */

Public void clean_up_entity_manager();
Public int32 add_entity();
Public Entity *get_entity(const int32 id);
Public void remove_entity(int32 id);

#endif // entity_H
