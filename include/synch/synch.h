#ifndef synch_H
#define synch_H

#include <elios.h>

#define ENTITY_MUTEX_ID 0

Elios_Public void *get_mutex(int32 mutexId);
Elios_Public void mutex_init(int32 mutexId);
Elios_Public void mutex_lock(int32 mutexId);
Elios_Public void mutex_unlock(int32 mutexId);
Elios_Public void mutex_destroy(int32 mutexId);

#endif // synch_H
