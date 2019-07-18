#include <boss.hpp>
#include "boss_mutex.hpp"

#if BOSS_WINDOWS
    #include <windows.h>
    #define MUTEX_DATA struct{HANDLE E; long C;}
    #define MUTEX_INIT(ID) do{(ID)->E = CreateEvent(0, 0, 0, 0); (ID)->C = 0;} while(false)
    #define MUTEX_DESTROY(ID) do{if((ID)->E) {CloseHandle((ID)->E); (ID)->E = 0;}} while(false)
    #define MUTEX_LOCK(ID) do{if(1 < InterlockedIncrement(&(ID)->C)) WaitForSingleObject((ID)->E, INFINITE);} while(false)
    #define MUTEX_UNLOCK(ID) do{if(0 < InterlockedDecrement(&(ID)->C)) SetEvent((ID)->E);} while(false)
#else
    #include <pthread.h>
    #define MUTEX_DATA pthread_mutex_t
    #define MUTEX_INIT(ID) pthread_mutex_init((ID), nullptr)
    #define MUTEX_DESTROY(ID) pthread_mutex_destroy((ID))
    #define MUTEX_LOCK(ID) pthread_mutex_lock((ID))
    #define MUTEX_UNLOCK(ID) pthread_mutex_unlock((ID))
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

    void Mutex::Lock(id_mutex mutex)
    {
        MUTEX_LOCK((MutexData*) mutex);
    }

    void Mutex::Unlock(id_mutex mutex)
    {
        MUTEX_UNLOCK((MutexData*) mutex);
    }
}
