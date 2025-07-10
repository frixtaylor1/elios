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

Elios_Private NativeMutex internal_mutex[MAX_COMPONENTS_PER_ENTITY + 1];

Elios_Public void *get_mutex(int32 mutexId) {
    return $void &internal_mutex[mutexId];
}

Elios_Public void mutex_init(int32 mutexId) {
    MUTEX_INIT((NativeMutex *)get_mutex(mutexId + 1));
}

Elios_Public void mutex_lock(int32 mutexId) {
    MUTEX_LOCK((NativeMutex *)get_mutex(mutexId + 1));
}

Elios_Public void mutex_unlock(int32 mutexId) {
    MUTEX_UNLOCK((NativeMutex *)get_mutex(mutexId + 1));
}

Elios_Public void mutex_destroy(int32 mutexId) {
    MUTEX_DESTROY((NativeMutex *)get_mutex(mutexId + 1));
}
