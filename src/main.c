#include <elios.h>
#include <entities/entity.h>
#include <stdio.h>

int main() {
	init_entity_manager();
	int32 id = add_entity();
	int32 id1 = add_entity();
	int32 id2 = add_entity();

	Entity *e = get_entity(id);
	Entity *e1 = get_entity(id1);
	Entity *e2 = get_entity(id2);

	add_component(e1, CMP_HEALTH);
	add_component(e1, CMP_COLLISION);
	add_component(e1, CMP_RENDER);

	inspect_entity(e);
	inspect_entity(e1);
	inspect_entity(e2);

	return 0;
}