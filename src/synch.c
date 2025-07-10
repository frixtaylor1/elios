#include <synch/synch.h>

#ifdef _WIN32
    #include <windows.h>
    typedef CRITICAL_SECTION NativeMutex;

    #define MUTEX_INIT(m)    InitializeCriticalSection((m))
    #define MUTEX_LOCK(m)    EnterCriticalSection((m))
    #define MUTEX_UNLOCK(m)  LeaveCriticalSection((m))
    #define MUTEX_DESTROY(m) DeleteCriticalSection((m))
#else
    #include <pthread.h>
    typedef pthread_mutex_t NativeMutex;

    #define MUTEX_INIT(m)    pthread_mutex_init((m), NULL)
    #define MUTEX_LOCK(m)    pthread_mutex_lock((m))
    #define MUTEX_UNLOCK(m)  pthread_mutex_unlock((m))
    #define MUTEX_DESTROY(m) pthread_mutex_destroy((m))
#endif

static NativeMutex entity_mutex_internal;
void *entity_mutex = &entity_mutex_internal;

Elios_Public void mutex_init(void *m) {
    MUTEX_INIT((NativeMutex *)m);
}

Elios_Public void mutex_lock(void *m) {
    MUTEX_LOCK((NativeMutex *)m);
}

Elios_Public void mutex_unlock(void *m) {
    MUTEX_UNLOCK((NativeMutex *)m);
}

Elios_Public void mutex_destroy(void *m) {
    MUTEX_DESTROY((NativeMutex *)m);
}
