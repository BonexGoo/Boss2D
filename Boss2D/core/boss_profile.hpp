#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

namespace BOSS
{
    //! \brief 프로파일링지원
    class Profile
    {
    public:
        /*!
        \brief 프로파일구간을 시작(즉시 랩지정 가능)
        \param groupname : 프로파일의 그룹명
        \param lapname : 랩지정시(lapname != nullptr) 랩명
        \param enable : 사용여부
        */
        static void Start(chars groupname, chars lapname = nullptr, bool enable = true);

        /*!
        \brief 프로파일구간을 종료
        */
        static void Stop();

        /*!
        \brief 랩을 지정
        \param name : 랩명
        */
        static void Lap(chars name);

        /*!
        \brief 현재 프로파일을 초기화
        */
        static void Clear();

        /*!
        \brief 현재 프로파일을 디버그창에 출력
        */
        static void DebugPrint();
    };
}
