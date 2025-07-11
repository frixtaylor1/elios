#include <threads/threads.h>
#include <entities/entity.h>
#include <components/components.h>
#include <stdio.h>

void health_system(int threadId, int start, int end) {
    for (int id = start; id < end; id++) {
        Entity *e = get_entity(id);
        IfTrue (has_component(e, CMP_HEALTH)) {
            HealthComponent *hc = get_component(e, CMP_HEALTH);
            hc->health -= 1.1f;
        }
    }
    (void) threadId;
}

void collision_system(int threadId, int start, int end) {
    for (int i = start; i < end; i++) {
        Entity *e = get_entity(i);
        IfTrue (has_component(e, CMP_COLLISION)) {
            CollisionComponent *cc = get_component(e, CMP_COLLISION);
            cc->isColliding = false;
            IfTrue (cc->posOfCollision.x > 100) {
                cc->isColliding = true;
            }
        }
    }
    (void) threadId;
}

int main() {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        int id = add_entity();
        Entity *e = get_entity(id);
        add_component(e, CMP_HEALTH, $void &(HealthComponent){.health = 100.f});
        add_component(e, CMP_COLLISION, $void &(CollisionComponent){
            .isColliding = false,
            .posOfCollision = {.x=121.f, .y=122.f}});
    }

    thread_pool_init();

    WhileTrue (true) {
        dispatch_system(&health_system);
        sync_threads();

        dispatch_system(&collision_system);
        sync_threads();
    }

    thread_pool_shutdown();
    return 0;
}
