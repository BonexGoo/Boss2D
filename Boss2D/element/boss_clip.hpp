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
        Clip(sint32 l, sint32 t, sint32 r, sint32 b, bool doScissor);
        ~Clip();

        Clip& operator=(const Clip& rhs);

        inline const sint32 Width() const {return r - l;}
        inline const sint32 Height() const {return b - t;}

    public:
        sint32 l;
        sint32 t;
        sint32 r;
        sint32 b;
        bool didscissor;
    };
    typedef Array<Clip, datatype_class_canmemcpy> Clips;
}
