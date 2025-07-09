#include <entities/entity.h>
#include <components/components.h>
#include <stdio.h>

int main() {
	int32 id = add_entity();
	int32 id1 = add_entity();
	int32 id2 = add_entity();

	Entity *e = get_entity(id);
	Entity *e1 = get_entity(id1);
	Entity *e2 = get_entity(id2);

	add_component(CMP_HEALTH, e1, $void &(HealthComponent) {.health = 100.f});
	add_component(CMP_COLLISION, e1, $void &(CollisionComponent) {.isColliding = true, .posOfCollision = (Vector2){.x = 100.f, .y = 100.f}});

	HealthComponent *hc = get_component(CMP_HEALTH, e1);
	CollisionComponent *cc = get_component(CMP_COLLISION, e1);


	printf("Health component: %f\n", hc->health);
	printf("Collision component: %i, %f, %f\n", cc->isColliding, cc->posOfCollision.x, cc->posOfCollision.y);
	inspect_entity(e);
	inspect_entity(e1);
	inspect_entity(e2);

	return 0;
}