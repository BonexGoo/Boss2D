#pragma once
#include <boss_mutex.hpp>

namespace BOSS
{
    /// @brief 카운터지원
    template<bool FORTHREAD>
    class Counter
    {
    public:
        /// @brief 카운트 증가/감소
        /// @param doAdd : 증가여부
        /// @see Get
        void Set(bool doAdd)
        {
            if(FORTHREAD) Mutex::Lock(Mutex);
            {
                Count += (doAdd)? 1 : -1;
            }
            if(FORTHREAD) Mutex::Unlock(Mutex);
        }

        /// @brief 현재 카운트값 얻기
        /// @return 카운트값
        /// @see Set
        sint32 Get()
        {
            sint32 Result = 0;
            if(FORTHREAD) Mutex::Lock(Mutex);
            {
                Result = Count;
            }
            if(FORTHREAD) Mutex::Unlock(Mutex);
            return Result;
        }

    public:
        Counter()
        {
            Count = 0;
            if(FORTHREAD) Mutex = Mutex::Open();
        }

        ~Counter()
        {
            if(FORTHREAD) Mutex::Close(Mutex);
        }

    private:
        sint32 Count;
        id_mutex Mutex;
    };
    typedef Counter<true> SafeCounter;
    typedef Counter<false> FastCounter;
}
