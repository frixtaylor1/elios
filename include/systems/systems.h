#ifndef system_H
#define system_H

#include <elios.h>

Elios_Public void health_system(int threadId, int start, int end);
Elios_Public void physics_system(int threadId, int start, int end);
Elios_Public void render_system();
Elios_Public void init_entities();
Elios_Public void destroy_entities();

#endif // system_H