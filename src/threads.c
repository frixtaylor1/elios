#include <threads/threads.h>
#include <stdio.h>
#include <stdlib.h>
#include <elios.h>

#define NUM_THREADS 4
#define CHUNK_SIZE (MAX_ENTITIES / NUM_THREADS)

typedef void (*SystemFunction)(int thread_id, int start, int end);

#ifdef _WIN32
    #include <windows.h>

    #define Native_Handle HANDLE
    #define Native_Handle_Event HANDLE
    #define Thread_Func DWORD WINAPI thread_func(LPVOID arg)

    #define native_wait_for_signal(hHandle) WaitForSingleObject(hHandle, INFINITE)

    #define native_init_threads(thread_fn, thread, start_event, done_event, thread_id_ptr)  \
        start_event = CreateEvent(NULL, FALSE, FALSE, NULL);                                \
        done_event  = CreateEvent(NULL, FALSE, FALSE, NULL);                                \
        thread      = CreateThread(NULL, 0, thread_fn, thread_id_ptr, 0, NULL)

    #define NativeSetEventFunctionType BOOL
    #define native_set_event SetEvent

    #define native_destroy_threads(thread, start_event, done_event) \
        WaitForSingleObject(thread, INFINITE);                      \
        CloseHandle(thread);                                        \
        CloseHandle(start_event);                                   \
        CloseHandle(done_event);

    #define native_sleep(ms) Sleep(ms)

#else
    #include <pthread.h>
    #include <semaphore.h>
    #include <unistd.h>

    #define Native_Handle pthread_t
    #define Native_Handle_Event sem_t
    #define Thread_Func void* thread_func(void *arg)

    #define native_wait_for_signal(sem) sem_wait(&sem)

    #define native_init_threads(thread_fn, thread, start_event, done_event, thread_id_ptr)  \
        sem_init(&start_event, 0, 0);                                                       \
        sem_init(&done_event, 0, 0);                                                        \
        pthread_create(&thread, NULL, thread_fn, thread_id_ptr)

    #define NativeSetEventFunctionType int
    #define native_set_event(sem) sem_post(&sem)

    #define native_destroy_threads(thread, start_event, done_event) \
        pthread_join(thread, NULL);                                 \
        sem_destroy(&start_event);                                  \
        sem_destroy(&done_event);

    #define native_sleep(ms) usleep(ms * 1000)
#endif

static Native_Handle       threads[NUM_THREADS];
static Native_Handle_Event start_events[NUM_THREADS];
static Native_Handle_Event done_events[NUM_THREADS];

static int thread_ids[NUM_THREADS];
static SystemFunction current_function = NULL;
static volatile int stop_threads = 0;

Thread_Func {
    int thread_id = *(int *)arg;

    WhileFalse (stop_threads) {
        native_wait_for_signal(start_events[thread_id]);

        IfTrue(stop_threads) break;

        int start = thread_id * CHUNK_SIZE;
        int end   = (thread_id + 1) * CHUNK_SIZE;
        IfTrue (end > MAX_ENTITIES) end = MAX_ENTITIES;

        IfTrue ((bool) current_function) {
            current_function(thread_id, start, end);
        }

        native_set_event(done_events[thread_id]);
    }

    return 0;
}

void thread_pool_init() {
    stop_threads = 0;
    ForRange (int, i, 0, NUM_THREADS)
        thread_ids[i] = i;
        native_init_threads(thread_func, threads[i], start_events[i], done_events[i], &thread_ids[i]);
    EForRange;
}

void dispatch_system(SystemFunction function) {
    current_function = function;
    ForRange (int, i, 0, NUM_THREADS)
        native_set_event(start_events[i]);
    EForRange;
}

void sync_threads() {
    ForRange (int, i, 0, NUM_THREADS)
        native_wait_for_signal(done_events[i]);
    EForRange;
    current_function = NULL;
}

void thread_pool_shutdown() {
    stop_threads = 1;
    ForRange (int, i, 0, NUM_THREADS)
        native_set_event(start_events[i]);
    EForRange;

    ForRange (int, i, 0, NUM_THREADS)
        native_destroy_threads(threads[i], start_events[i], done_events[i]);
    EForRange;
}

void thread_sleep(int32 value) {
    native_sleep(value);
}
