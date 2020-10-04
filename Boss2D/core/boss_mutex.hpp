#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

namespace BOSS
{
    BOSS_DECLARE_ID(id_mutex);

    /// @brief 뮤텍스함수지원
    class Mutex
    {
    public:
        /// @brief 뮤텍스 열기
        /// @return 생성된 뮤텍스ID
        /// @see Close
        static id_mutex Open();

        /// @brief 뮤텍스 닫기
        /// @param mutex : 뮤텍스ID
        /// @see Open
        static void Close(id_mutex mutex);

        /// @brief 뮤텍스 잠금여부
        /// @param mutex : 뮤텍스ID
        /// @return 잠금여부(true-잠금상태, false-해제상태)
        /// @see Lock
        static bool HasLock(id_mutex mutex);

        /// @brief 뮤텍스 잠금
        /// @param mutex : 뮤텍스ID
        /// @see Unlock
        static void Lock(id_mutex mutex);

        /// @brief 뮤텍스 해제
        /// @param mutex : 뮤텍스ID
        /// @see Lock
        static void Unlock(id_mutex mutex);
    };
}
