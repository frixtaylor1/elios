#ifndef entity_H
#define entity_H

#include "../elios.h"

typedef struct {
  mask components;
  int32 id: 30;
  bool reserved : 1;
  bool alloced : 1;
  bool toRemove : 1;
} Entity;

typedef struct {
  Entity entities[MAX_ENTITIES];
  int32 nbEntities;
  int32 idxCounter;
} EntityManager;

typedef void (*ActionComponent)(void *component);

/*
 * Entity functions...
 */

Elios_Public void for_each_component_of_entity(const Entity *entity, ActionComponent callback);
Elios_Public void add_component(Entity *entity, int32 component, void *content);
Elios_Public void *get_component(const Entity *entity, int32 type);
Elios_Public void remove_component(Entity *entity, int32 component);
Elios_Public bool has_component(const Entity *entity, int32 component);
Elios_Public void inspect_entity(const Entity *entity);

/*
 * EntityManager functions...
 */

Elios_Public void    clean_up_entity_manager();
Elios_Public int32   add_entity();
Elios_Public Entity *get_entity(const int32 id);
Elios_Public void    remove_entity(const int32 id);
Elios_Public void    mark_to_remove_entity(const int32 id);
Elios_Public int32   get_nb_entities();
Elios_Public void    init_entities();
Elios_Public void    destroy_entities();
Elios_Public void    save_entities_state();
Elios_Public void    reload_entities_state();

#endif // entity_H
