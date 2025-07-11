#ifndef threads_H
#define threads_H

#include <elios.h>

#define NUM_THREADS 4
#define CHUNK_SIZE (MAX_ENTITIES / NUM_THREADS)

typedef void (*SystemFunction)(int thread_id, int start, int end);

Elios_Public void thread_pool_init();
Elios_Public void dispatch_system(SystemFunction function);
Elios_Public void sync_threads();
Elios_Public void thread_pool_shutdown();
Elios_Public void thread_sleep(int32 value);

#endif // threads_H