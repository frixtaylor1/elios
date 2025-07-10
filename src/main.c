#include <entities/entity.h>
#include <components/components.h>
#include <stdio.h>

int main() {
	int32 id  = add_entity();
	int32 id1 = add_entity();
	int32 id2 = add_entity();

	Entity *e  = get_entity(id);
	Entity *e1 = get_entity(id1);
	Entity *e2 = get_entity(id2);

	add_component(e1, CMP_HEALTH, $void &(HealthComponent) {.health = 100.f});
	add_component(e1, CMP_COLLISION, $void &(CollisionComponent) {.isColliding = true, .posOfCollision = (Vector2){.x = 121.f, .y = 122.f}});
	add_component(e1, CMP_RENDER, $void &(RenderComponent) {.pos = (Vector2){.x=10.f, .y=10.f}});

	HealthComponent *hc = get_component(e1, CMP_HEALTH);
	RenderComponent *rc = get_component(e1, CMP_RENDER);
	CollisionComponent *cc = get_component(e1, CMP_COLLISION);

	printf("CollisionComponent values: %f, %f\n", cc->posOfCollision.x, cc->posOfCollision.y);
	printf("HealthComponent values: %f \n", hc->health);
	printf("RenderComponent values: %f, %f\n", rc->pos.x, rc->pos.y);

	//remove_entity(e1->id);

	inspect_entity(e);
	inspect_entity(e1);
	inspect_entity(e2);

	return 0;
}