#include <threads/threads.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>

    Elios_Private HANDLE threads[NUM_THREADS];
    Elios_Private HANDLE start_events[NUM_THREADS];
    Elios_Private HANDLE done_events[NUM_THREADS];
#else
    #include <pthread.h>
    #include <semaphore.h>
    #include <unistd.h>

    Elios_Private pthread_t threads[NUM_THREADS];
    Elios_Private sem_t start_sems[NUM_THREADS];
    Elios_Private sem_t done_sems[NUM_THREADS];
#endif

Elios_Private int thread_ids[NUM_THREADS];
Elios_Private SystemFunction current_function = NULL;
Elios_Private volatile int stop_threads = 0;

#ifdef _WIN32
DWORD WINAPI thread_func(LPVOID arg)
#else
void* thread_func(void *arg)
#endif
{
    int thread_id = *(int*)arg;

    while (!stop_threads) {
        #ifdef _WIN32
            WaitForSingleObject(start_events[thread_id], INFINITE);
        #else
            sem_wait(&start_sems[thread_id]);
        #endif

        if (stop_threads) break;

        int start = thread_id * CHUNK_SIZE;
        int end = start + CHUNK_SIZE;

        if (current_function) current_function(thread_id, start, end);

        #ifdef _WIN32
            SetEvent(done_events[thread_id]);
        #else
            sem_post(&done_sems[thread_id]);
        #endif
    }

    return 0;
}
Elios_Public void thread_pool_init() {
    stop_threads = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;

        #ifdef _WIN32
            start_events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
            done_events[i]  = CreateEvent(NULL, FALSE, FALSE, NULL);
            threads[i] = CreateThread(NULL, 0, thread_func, &thread_ids[i], 0, NULL);
        #else
            sem_init(&start_sems[i], 0, 0);
            sem_init(&done_sems[i], 0, 0);
            pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        #endif
    }
}

Elios_Public void dispatch_system(SystemFunction callback) {
    current_function = callback;
    for (int i = 0; i < NUM_THREADS; i++) {
        #ifdef _WIN32
            SetEvent(start_events[i]);
        #else
            sem_post(&start_sems[i]);
        #endif
    }
}

Elios_Public void sync_threads() {
    for (int i = 0; i < NUM_THREADS; i++) {
        #ifdef _WIN32
            WaitForSingleObject(done_events[i], INFINITE);
        #else
            sem_wait(&done_sems[i]);
        #endif
    }
    current_function = NULL;
}

Elios_Public void thread_pool_shutdown() {
    stop_threads = 1;

    for (int i = 0; i < NUM_THREADS; i++) {
        #ifdef _WIN32
            SetEvent(start_events[i]);
        #else
            sem_post(&start_sems[i]);
        #endif
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        #ifdef _WIN32
            WaitForSingleObject(threads[i], INFINITE);
            CloseHandle(threads[i]);
            CloseHandle(start_events[i]);
            CloseHandle(done_events[i]);
        #else
            pthread_join(threads[i], NULL);
            sem_destroy(&start_sems[i]);
            sem_destroy(&done_sems[i]);
        #endif
    }
}

Elios_Public void thread_sleep(int32 value) {
    #ifdef _WIN32
        Sleep(value);
    #else
        usleep(value * 1000);
    #endif
}