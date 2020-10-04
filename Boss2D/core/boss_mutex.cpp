#include <boss.hpp>
#include "boss_mutex.hpp"

#if BOSS_WINDOWS
    #include <windows.h>
    #define MUTEX_DATA struct {HANDLE E; long C; bool F;}
    #define MUTEX_INIT(ID) do { \
        (ID)->E = CreateEvent(0, 0, 0, 0); \
        (ID)->C = 0; \
        (ID)->F = false; \
    } while(false)
    #define MUTEX_DESTROY(ID) do { \
        if((ID)->E) { \
            CloseHandle((ID)->E); \
            (ID)->E = 0; \
        } \
    } while(false)
    #define MUTEX_HAS_LOCK(ID) ((ID)->F)
    #define MUTEX_LOCK(ID) do { \
        (ID)->F = true; \
        if(1 < InterlockedIncrement(&(ID)->C)) \
            WaitForSingleObject((ID)->E, INFINITE); \
    } while(false)
    #define MUTEX_UNLOCK(ID) do { \
        if(0 < InterlockedDecrement(&(ID)->C)) \
            SetEvent((ID)->E); \
        (ID)->F = false; \
    } while(false)
#else
    #include <pthread.h>
    #define MUTEX_DATA struct {pthread_mutex_t M; bool F;}
    #define MUTEX_INIT(ID) do { \
        pthread_mutex_init(&(ID)->M, nullptr); \
        (ID)->F = false; \
    } while(false)
    #define MUTEX_DESTROY(ID) do { \
        pthread_mutex_destroy(&(ID)->M); \
    } while(false)
    #define MUTEX_HAS_LOCK(ID) ((ID)->F)
    #define MUTEX_LOCK(ID) do { \
        (ID)->F = true; \
        pthread_mutex_lock(&(ID)->M); \
    } while(false)
    #define MUTEX_UNLOCK(ID) do { \
        pthread_mutex_unlock(&(ID)->M); \
        (ID)->F = false; \
    } while(false)
#endif
typedef MUTEX_DATA MutexData;

namespace BOSS
{
    id_mutex Mutex::Open()
    {
        MutexData* NewMutex = new MutexData();
        MUTEX_INIT(NewMutex);
        return (id_mutex) NewMutex;
    }

    void Mutex::Close(id_mutex mutex)
    {
        MUTEX_DESTROY((MutexData*) mutex);
    }

    bool Mutex::HasLock(id_mutex mutex)
    {
        return MUTEX_HAS_LOCK((MutexData*) mutex);
    }

    void Mutex::Lock(id_mutex mutex)
    {
        MUTEX_LOCK((MutexData*) mutex);
    }

    void Mutex::Unlock(id_mutex mutex)
    {
        MUTEX_UNLOCK((MutexData*) mutex);
    }
}
