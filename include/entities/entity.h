#ifndef entity_H
#define entity_H

#include "../elios.h"

#define MAX_ENTITIES 1024

typedef struct {
  mask components;
  int32 id: 30;
  bool reserved : 1;
  bool alloced : 1;
} Entity;

typedef struct {
  enum { Null_Entity = 0, Max_Entities = MAX_ENTITIES } EntityManagerState;
  Entity entities[Max_Entities];
  int32 nbEntities;
  int32 idxCounter;
} EntityManager;

/*
 * Entity functions...
 */

Public void add_component(Entity *entity, ComponentType component);
Public void remove_component(Entity *entity, ComponentType component);
Public bool has_component(Entity *entity, ComponentType component);
Public void inspect_entity(Entity *entity);

/*
 * EntityManager functions...
 */

Public void  init_entity_manager();
Public void clean_up_entity_manager();
Public int32 add_entity();
Public Entity *get_entity(int32 id);
Public void remove_entity(int32 id);

#endif // entity_H
