#pragma once
#include <platform/boss_platform.hpp>

namespace BOSS
{
    /// @brief 소스코드생성기
    class Typolar
    {
    public:
        static void BuildTypes(chars matchname, chars assetpath, chars outpath);
    };
}
