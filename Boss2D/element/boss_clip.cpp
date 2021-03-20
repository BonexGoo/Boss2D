#include <boss.hpp>
#include "boss_clip.hpp"

namespace BOSS
{
    Clip::Clip()
    {
        l = 0;
        t = 0;
        r = 0;
        b = 0;
        didscissor = false;
    }

    Clip::Clip(const Clip& rhs)
    {
        operator=(rhs);
    }

    Clip::Clip(sint32 l, sint32 t, sint32 r, sint32 b, bool doScissor)
    {
        this->l = l;
        this->t = t;
        this->r = r;
        this->b = b;
        this->didscissor = doScissor;
    }

    Clip::~Clip()
    {
    }

    Clip& Clip::operator=(const Clip& rhs)
    {
        l = rhs.l;
        t = rhs.t;
        r = rhs.r;
        b = rhs.b;
        didscissor = rhs.didscissor;
        return *this;
    }
}
