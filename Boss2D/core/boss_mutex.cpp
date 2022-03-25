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
        if(MUTEX_HAS_LOCK((MutexData*) mutex))
            BOSS_TRACE("Mutex에 의해 스레드가 중지됩니다");
        MUTEX_LOCK((MutexData*) mutex);
    }

    void Mutex::Unlock(id_mutex mutex)
    {
        MUTEX_UNLOCK((MutexData*) mutex);
    }

    static sint32 LocalCounter(id_mutex mutex, bool push)
    {
        class StackMutex
        {
        public:
            StackMutex() {mMutex = Mutex::Open();}
            ~StackMutex() {Mutex::Close(mMutex);}
        public:
            id_mutex mMutex;
        };
        static StackMutex Stack;

        sint32 Result = 0;
        Mutex::Lock(Stack.mMutex);
        {
            Map<sint32>& CounterPool = *BOSS_STORAGE_SYS(Map<sint32>);
            if(auto CurCounter = CounterPool.Access((uint64)(void*) mutex))
            {
                Result = (*CurCounter += (push)? 1 : -1);
                if(Result == 0)
                    CounterPool.Remove((uint64)(void*) mutex);
            }
            else
            {
                Result = (push)? 1 : -1;
                CounterPool[(uint64)(void*) mutex] = Result;
            }
        }
        Mutex::Unlock(Stack.mMutex);
        return Result;
    }

    void Mutex::LocalLock(id_mutex mutex)
    {
        if(LocalCounter(mutex, true) == 1)
        {
            BOSS_ASSERT("Mutex에 의해 스레드가 중지됩니다", !MUTEX_HAS_LOCK((MutexData*) mutex));
            MUTEX_LOCK((MutexData*) mutex);
        }
    }

    void Mutex::LocalUnlock(id_mutex mutex)
    {
        if(LocalCounter(mutex, false) == 0)
            MUTEX_UNLOCK((MutexData*) mutex);
    }
}
