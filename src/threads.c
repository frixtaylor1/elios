#include <threads/threads.h>
#include <stdio.h>
#include <stdlib.h>
#include <elios.h>

#define NUM_THREADS 4
#define CHUNK_SIZE (MAX_ENTITIES / NUM_THREADS)

typedef void (*SystemFunction)(int thread_id, int start, int end);

Elios_Public void thread_pool_init();
Elios_Public void dispatch_system(SystemFunction function);
Elios_Public void sync_threads();
Elios_Public void thread_pool_shutdown();
Elios_Public void thread_sleep(int32 value);

#ifdef _WIN32
    #include <windows.h>

    #define Native_Handle HANDLE
    #define Native_Handle_Event HANDLE
    #define Thread_Func DWORD WINAPI thread_func(LPVOID arg)

    #define native_wait_for_signal(hHandle) WaitForSingleObject(hHandle, INFINITE)

    #define native_init_threads(thread_fn, thread, start_event, done_event, thread_id)  \
        native_wait_for_signal(thread);                                                 \
        start_event = CreateEvent(NULL, FALSE, FALSE, NULL);                            \
        done_event  = CreateEvent(NULL, FALSE, FALSE, NULL);                            \
        thread      = CreateThread(NULL, 0, thread_fn, &thread_id, 0, NULL);

    #define NativeSetEventFunctionType BOOL
    #define native_set_event SetEvent

    #define native_destroy_threads(thread, start_event, done_event) \
        WaitForSingleObject(thread, INFINITE);                      \
        CloseHandle(thread);                                        \
        CloseHandle(start_event);                                   \
        CloseHandle(done_event);

    #define native_sleep Sleep

#else
    #include <pthread.h>
    #include <semaphore.h>
    #include <unistd.h>

    #define Native_Handle pthread_t
    #define Native_Handle_Event sem_t
    #define Thread_Func void* thread_func(void *arg)

    #define native_wait_for_signal(sem) sem_wait(&sem)

    #define native_init_threads(thread_fn, thread, start_event, done_event, thread_id)  \
        sem_init(&start_event, 0, 0);                                                   \
        sem_init(&done_event, 0, 0);                                                    \
        pthread_create(thread, NULL, thread_fn, &thread_id);

    #define NativeSetEventFunctionType int
    #define native_set_event(sem) sem_post(&sem)

    #define native_destroy_threads(thread, start_event, done_event) \
        pthread_join(thread, NULL);                                 \
        sem_destroy(&start_event);                                  \
        sem_destroy(&done_event);

    #define native_sleep(value) usleep(value * 1000)
#endif

Elios_Private Native_Handle       threads[NUM_THREADS];
Elios_Private Native_Handle_Event start_events[NUM_THREADS];
Elios_Private Native_Handle_Event done_events[NUM_THREADS];

typedef NativeSetEventFunctionType (*NativeEventCallback)(Native_Handle_Event);
typedef NativeSetEventFunctionType (*NativeCallback)(Native_Handle);
#define for_all_handles_and_events(execute) for (int idx = 0; idx < NUM_THREADS; idx++) { execute(threads[idx], start_events[idx], done_events[idx]); }

Elios_Private void for_each_native_handle(NativeCallback callback) {
    ForEach (Native_Handle *, thread, threads) {
        callback(thread);
    } EForEach;
}

Elios_Private void for_each_native_handle_event(Native_Handle_Event *events, NativeEventCallback callback) {
    Native_Handle_Event *event = events;
    WhileTrue ($void ++event < $void &events[NUM_THREADS]) {
        callback(event);
    }
}

Elios_Private int thread_ids[NUM_THREADS];
Elios_Private SystemFunction current_function = NULL;
Elios_Private volatile int stop_threads = 0;

Elios_Private Thread_Func {
    int thread_id = *(int*)arg;

    WhileFalse (stop_threads) {
        native_wait_for_signal(start_events[thread_id]);

        IfTrue (stop_threads) break;

        int start = thread_id * CHUNK_SIZE;
        int end   = start + CHUNK_SIZE;

        IfTrue (!!current_function) current_function(thread_id, start, end);

        native_set_event(&done_events[thread_id]);
    }

    return 0;
}

Elios_Public void thread_pool_init() {
    stop_threads = 0;

    int i = 0;
    WhileTrue (++i < NUM_THREADS) {
        thread_ids[i] = i;
        native_init_threads(thread_func, threads[i], start_events[i], done_events[i], thread_ids[i]);
    }
}
 
Elios_Public void dispatch_system(SystemFunction callback) {
    current_function = callback;
    for_each_native_handle_event(start_events, &native_set_event);
}

Elios_Public void sync_threads() {
    for_each_native_handle_event(start_events, &native_set_event);
    for (int i = 0; i < NUM_THREADS; i++) {
        native_wait_for_signal(done_events[i]);
    }
    current_function = NULL;
}

Elios_Public void thread_pool_shutdown() {
    stop_threads = 1;

    for_each_native_handle(&native_set_event);
    for_all_handles_and_events(native_destroy_threads);
}

Elios_Public void thread_sleep(int32 value) {
    native_sleep(value);
}