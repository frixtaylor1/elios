#ifndef synch_H
#define synch_H

#include <elios.h>

extern void *entity_mutex;

Elios_Public void mutex_init(void *m);
Elios_Public void mutex_lock(void *m);
Elios_Public void mutex_unlock(void *m);
Elios_Public void mutex_destroy(void *m);

#endif // synch_H
