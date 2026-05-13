#pragma once
#include <element/boss_size.hpp>

namespace BOSS
{
    /// @brief 2D클리퍼
    class Clip
    {
    public:
        Clip();
        Clip(const Clip& rhs);
        Clip(float l, float t, float r, float b, bool doScissor);
        ~Clip();

        Clip& operator=(const Clip& rhs);

        inline const float Width() const {return r - l;}
        inline const float Height() const {return b - t;}

    public:
        float l;
        float t;
        float r;
        float b;
        bool didscissor;
    };
    typedef Array<Clip, datatype_class_canmemcpy> Clips;
}
