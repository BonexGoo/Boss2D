#pragma once
#include <platform/boss_platform.hpp>
#include <platform/boss_platform_impl.hpp>

#if !defined(BOSS_PLATFORM_QT5) && !defined(BOSS_PLATFORM_QT6) && !defined(BOSS_PLATFORM_COCOS2DX) && !defined(BOSS_PLATFORM_NATIVE)

    class ViewAPI
    {
    public:
        ViewAPI() {}
        ~ViewAPI() {}

    public:
        static inline dependency*& CurPainter()
        {static dependency* _ = nullptr; return _;}
        static inline dependency& CurColor()
        {static dependency _; return _;}
    };

#endif
