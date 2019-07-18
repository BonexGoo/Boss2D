#include <boss.hpp>
#include "boss_math.hpp"

#include <math.h>
#include <platform/boss_platform.hpp>

namespace BOSS
{
    float Math::MinF(const float a, const float b)
    {
        return fminf(a, b);
    }

    float Math::MaxF(const float a, const float b)
    {
        return fmaxf(a, b);
    }

    sint32 Math::Cycle(const sint32 v, const sint32 low, const sint32 len)
    {
        BOSS_ASSERT("len은 0보다 커야 합니다", 0 < len);
        const sint32 C = (v - low) % len;
        return C + low + len * (C < 0);
    }

    float Math::CycleF(const float v, const float low, const float len)
    {
        BOSS_ASSERT("len은 0보다 커야 합니다", 0 < len);
        const float C = fmodf(v - low, len);
        return C + low + len * (C < 0);
    }

    float Math::Ceil(const float v)
    {
        return ceilf(v);
    }

    float Math::Floor(const float v)
    {
        return floorf(v);
    }

    float Math::Round(const float v)
    {
        return roundf(v);
    }

    float Math::Mod(const float a, const float b)
    {
        return fmodf(a, b);
    }

    float Math::Sqrt(const float v)
    {
        return sqrtf(v);
    }

    float Math::Cos(const float rad)
    {
        return cosf(rad);
    }

    float Math::Sin(const float rad)
    {
        return sinf(rad);
    }

    float Math::Tan(const float rad)
    {
        return tanf(rad);
    }

    float Math::Atan(const float x, const float y)
    {
        return atan2f(y, x);
    }

    float Math::Distance(const float x1, const float y1, const float x2, const float y2)
    {
        const float X = x1 - x2;
        const float Y = y1 - y2;
        return sqrtf(X * X + Y * Y);
    }

    float Math::Random()
    {
        return ((sint32) (Platform::Utility::Random() & 0x7FFFFFFF)) / (float) 0x7FFFFFFF;
    }
}
